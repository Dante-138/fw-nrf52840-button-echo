# fw-nrf52840-button-echo

Button detection and UART echo on the nRF52840 DK. Zephyr RTOS, nRF Connect SDK.

## What it does

- Button detection: the four DK buttons are GPIO inputs with an interrupt on the active edge (`GPIO_INT_EDGE_TO_ACTIVE`). One callback handles all of them and identifies the source by bitmask, then prints a line such as `Botao 1 pressionado!`.
- UART echo: interrupt-driven UART (`CONFIG_UART_INTERRUPT_DRIVEN=y`). The RX callback reads the FIFO and echoes through `uart_poll_out()`. `\r` is echoed with `\n` added.

Startup banner:

```
=================================
  nRF52840 DK - Botoes + Echo
=================================
Pressione os botoes 1-4 na DK.
Digite algo para testar o echo.
```

The messages printed by the firmware are in Portuguese.

## Hardware

| Item | Detail |
|---|---|
| Board | nRF52840 DK (PCA10056) |
| MCU | nRF52840, Cortex-M4F, 64 MHz |
| UART | J-Link VCOM over USB, 115200 8N1, no flow control |

| Button | GPIO | Devicetree alias |
|---|---|---|
| SW1 | P0.11 | `sw0` |
| SW2 | P0.12 | `sw1` |
| SW3 | P0.24 | `sw2` |
| SW4 | P0.25 | `sw3` |

## Build

Requires nRF Connect SDK v2.7.0 or newer, including v3.x.

```bash
west build -b nrf52840dk/nrf52840 . --no-sysbuild
```

`--no-sysbuild` is needed on SDK v3.x.

## Flash

```bash
west flash
```

## Test

1. Open a serial terminal on the DK's COM port: 115200 baud, 8N1, no flow control.
2. Press SW1 to SW4: each press prints the matching message.
3. Type text: it is echoed back.

## License

MIT, see [LICENSE](LICENSE).
