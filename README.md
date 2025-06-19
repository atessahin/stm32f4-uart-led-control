# LED Control System via UART (STM32F4)

## Description
A basic LED control system using STM32F411 and UART.  
The board receives `"on"` or `"off"` commands via serial and toggles the LED accordingly.  
Useful for learning UART, GPIO, and embedded C basics.

## Features
- UART-based LED control commands: `on`, `off`
- Onboard LED feedback
- Basic clock, GPIO, and UART initialization

## How to Use
1. Flash the code to the STM32F411E Discovery board.
2. Open a serial terminal at **115200 baud**, 8N1 format.
3. Type `on` or `off` and press Enter.
4. LED will toggle based on the command and show a response on terminal.
