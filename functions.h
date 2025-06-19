#include "stm32f4xx.h"                  // Device header

#include <string.h>
void systemClockConfig();
void gpioConfig();
void uartConfig();
void UART2_SendChar(char ch);
void UART2_SendString(char *str);
void delay(uint32_t time);
void UART2_ReceiveString(char *buffer, uint16_t maxLen);
int UART2_DataAvailable(void);
uint8_t UART2_ReceiveChar(void);
void turnOffLed();
void turnLed();