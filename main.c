#include "functions.h"

int main(void)
{
    systemClockConfig();
    
    gpioConfig();
    
    uartConfig();
    
    char buffer[50];
    
    UART2_SendString("\r\nLED Control System\r\n");
    UART2_SendString("Commands: on, off\r\n");
    
    while(1)
    {

        UART2_ReceiveString(buffer, 50);
      
        if(strcmp(buffer, "on") == 0)
        {
            turnLed();  
            UART2_SendString("\r\nLED is ON!\r\n"); 
        }
        else if(strcmp(buffer, "off") == 0)
        {
            turnOffLed();
            UART2_SendString("\r\nLED is OFF!\r\n");  
        }
    }
}