/*
 * Firmware nRF52840 DK - Botões + Echo UART
 *
 * - Imprime no terminal (PuTTY) qual botão foi pressionado (1-4)
 * - Ecoa de volta qualquer caractere digitado no terminal
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

/* ---------- Botões (sw0..sw3 do devicetree da DK) ---------- */
#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
#define SW2_NODE DT_ALIAS(sw2)
#define SW3_NODE DT_ALIAS(sw3)

static const struct gpio_dt_spec buttons[] = {
	GPIO_DT_SPEC_GET(SW0_NODE, gpios),
	GPIO_DT_SPEC_GET(SW1_NODE, gpios),
	GPIO_DT_SPEC_GET(SW2_NODE, gpios),
	GPIO_DT_SPEC_GET(SW3_NODE, gpios),
};

#define NUM_BUTTONS ARRAY_SIZE(buttons)

static struct gpio_callback button_cb_data[NUM_BUTTONS];

/* Callback genérico — identifica qual botão gerou a interrupção */
static void button_pressed_cb(const struct device *dev,
			      struct gpio_callback *cb,
			      uint32_t pins)
{
	for (int i = 0; i < NUM_BUTTONS; i++) {
		if (pins & BIT(buttons[i].pin)) {
			printk("Botao %d pressionado!\r\n", i + 1);
		}
	}
}

static int buttons_init(void)
{
	int ret;

	for (int i = 0; i < NUM_BUTTONS; i++) {
		if (!gpio_is_ready_dt(&buttons[i])) {
			printk("Erro: GPIO do botao %d nao esta pronto\r\n", i + 1);
			return -ENODEV;
		}

		ret = gpio_pin_configure_dt(&buttons[i], GPIO_INPUT);
		if (ret < 0) {
			printk("Erro ao configurar botao %d: %d\r\n", i + 1, ret);
			return ret;
		}

		ret = gpio_pin_interrupt_configure_dt(&buttons[i],
						      GPIO_INT_EDGE_TO_ACTIVE);
		if (ret < 0) {
			printk("Erro ao configurar interrupcao do botao %d: %d\r\n",
			       i + 1, ret);
			return ret;
		}

		gpio_init_callback(&button_cb_data[i], button_pressed_cb,
				   BIT(buttons[i].pin));
		gpio_add_callback(buttons[i].port, &button_cb_data[i]);
	}

	return 0;
}

/* ---------- Echo UART ---------- */
static const struct device *const uart_dev =
	DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

static void uart_rx_cb(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	if (!uart_irq_update(dev)) {
		return;
	}

	if (uart_irq_rx_ready(dev)) {
		uint8_t c;

		while (uart_fifo_read(dev, &c, 1) == 1) {
			/* Ecoa o caractere de volta */
			uart_poll_out(dev, c);

			/* Se Enter, envia também nova linha */
			if (c == '\r') {
				uart_poll_out(dev, '\n');
			}
		}
	}
}

static int uart_echo_init(void)
{
	if (!device_is_ready(uart_dev)) {
		printk("Erro: UART nao esta pronta\r\n");
		return -ENODEV;
	}

	uart_irq_callback_set(uart_dev, uart_rx_cb);
	uart_irq_rx_enable(uart_dev);

	return 0;
}

/* ---------- Main ---------- */
int main(void)
{
	printk("\r\n=================================\r\n");
	printk("  nRF52840 DK - Botoes + Echo\r\n");
	printk("=================================\r\n");
	printk("Pressione os botoes 1-4 na DK.\r\n");
	printk("Digite algo para testar o echo.\r\n\r\n");

	if (buttons_init() < 0) {
		printk("Falha ao inicializar botoes!\r\n");
		return -1;
	}

	if (uart_echo_init() < 0) {
		printk("Falha ao inicializar echo UART!\r\n");
		return -1;
	}

	/* Loop principal — botões e UART são tratados por interrupção */
	while (1) {
		k_sleep(K_FOREVER);
	}

	return 0;
}
