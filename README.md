# nRF52840 DK - Button Detection + UART Echo

Firmware para a **nRF52840 DK** desenvolvido com **Zephyr RTOS** (nRF Connect SDK).

## Funcionalidades

### 1. Detecao de Botoes (1-4)
Detecta o pressionamento dos 4 botoes da DK via interrupcao GPIO e imprime no terminal serial qual botao foi pressionado.

```
Botao 1 pressionado!
Botao 2 pressionado!
Botao 3 pressionado!
Botao 4 pressionado!
```

### 2. Echo UART
Qualquer caractere digitado no terminal serial (PuTTY, minicom, etc.) e ecoado de volta imediatamente. Funciona via interrupcao de RX na UART.

## Hardware

| Componente | Descricao |
|---|---|
| **Placa** | nRF52840 DK (PCA10056) |
| **MCU** | nRF52840 (ARM Cortex-M4F, 64 MHz) |
| **Botoes** | SW1-SW4 (GPIO com pull-up interno) |
| **UART** | USB CDC ACM via J-Link onboard |

### Pinagem dos Botoes (nRF52840 DK)

| Botao | Pino GPIO | Alias no Devicetree |
|---|---|---|
| Button 1 (SW1) | P0.11 | `sw0` |
| Button 2 (SW2) | P0.12 | `sw1` |
| Button 3 (SW3) | P0.24 | `sw2` |
| Button 4 (SW4) | P0.25 | `sw3` |

## Pre-requisitos

- [nRF Connect SDK](https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-SDK) v2.7.0+ ou v3.x
- [nRF Connect for Desktop](https://www.nordicsemi.com/Products/Development-tools/nRF-Connect-for-Desktop) com Toolchain Manager
- nRF52840 DK conectada via USB
- Terminal serial (PuTTY, minicom, Tera Term, etc.)

## Estrutura do Projeto

```
nrf52840_button_echo/
├── CMakeLists.txt      # Configuracao de build CMake
├── prj.conf            # Configuracao do Zephyr (Kconfig)
├── README.md
├── .gitignore
└── src/
    └── main.c          # Codigo fonte principal
```

## Como Compilar e Gravar

### 1. Abra o terminal do nRF Connect SDK

Use o terminal integrado do **Toolchain Manager** ou da extensao **nRF Connect** no VS Code.

### 2. Compile o projeto

```bash
cd /caminho/para/nrf52840_button_echo
west build -b nrf52840dk/nrf52840 . --no-sysbuild
```

> **Nota:** Use `--no-sysbuild` se estiver usando nRF Connect SDK v3.x.

### 3. Grave na placa

```bash
west flash
```

### 4. Conecte o terminal serial

Abra o PuTTY (ou outro terminal serial) com as seguintes configuracoes:

| Parametro | Valor |
|---|---|
| **Porta** | COMx (verifique no Gerenciador de Dispositivos) |
| **Baud Rate** | 115200 |
| **Data Bits** | 8 |
| **Stop Bits** | 1 |
| **Parity** | None |
| **Flow Control** | None |

## Saida Esperada

Ao abrir o terminal, voce vera:

```
=================================
  nRF52840 DK - Botoes + Echo
=================================
Pressione os botoes 1-4 na DK.
Digite algo para testar o echo.
```

- Pressione qualquer botao (1-4) na DK para ver a mensagem correspondente
- Digite qualquer texto no terminal para ver o echo

## Detalhes Tecnicos

### Botoes
- Configurados como **GPIO Input** com interrupcao na borda ativa (`GPIO_INT_EDGE_TO_ACTIVE`)
- Pull-up interno habilitado via devicetree da DK
- Callback unico trata todos os 4 botoes, identificando qual gerou a interrupcao via bitmask

### UART Echo
- Usa **UART interrupt-driven** (`CONFIG_UART_INTERRUPT_DRIVEN=y`)
- Callback de RX le caracteres do FIFO e ecoa imediatamente via `uart_poll_out()`
- Trata `\r` (Enter) adicionando `\n` para nova linha correta no terminal

### Configuracoes Kconfig (prj.conf)

```
CONFIG_GPIO=y                  # Habilita driver GPIO
CONFIG_SERIAL=y                # Habilita driver serial
CONFIG_CONSOLE=y               # Habilita console
CONFIG_UART_CONSOLE=y          # Console via UART
CONFIG_PRINTK=y                # Habilita printk()
CONFIG_UART_INTERRUPT_DRIVEN=y # UART por interrupcao (para echo)
```

## Licenca

MIT License
