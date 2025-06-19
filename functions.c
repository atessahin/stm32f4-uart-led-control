#include "functions.h"

void systemClockConfig()
{
	RCC->CR |=(1<<16);              // HSE ON
	while(!(RCC->CR & (1<<17)));    // Wait until HSE is ready
	
	FLASH->ACR|=(3<<1)|(1<<8)|(1<<9)|(1<<10);
	// Flash latency: 3 wait states
	// Enable prefetch buffer
	// Enable instruction cache
	// Enable data cache
	
	RCC->APB1ENR|=(1<<28);          // Power interface clock enable
	PWR->CR |=(3U<<14);             // Voltage regulator scale 1 mode
	
	RCC->CR &=~(1<<24);             // Disable PLL
	while(RCC->CR & (1<<25))        // Wait until PLL is unlocked
		;
	
	// Configure PLL: HSE/M * N/P = 8MHz/8 * 168/2 = 168MHz
	RCC->PLLCFGR = (8 << 0) |       // PLLM = 8 (bits 5:0)
                   (168 << 6) |     // PLLN = 168 (bits 14:6)
                   (0 << 16) |      // PLLP = 2 (bits 17:16) 00=÷2
                   (1 << 22);       // PLLSRC = HSE (bit 22)

	RCC->CR |= (1 << 24);           // PLL ON
	while(!(RCC->CR & (1 << 25)));  // Wait until PLL is ready	

	RCC->CFGR&=~(15U<<4);           // AHB prescaler = 1, 168MHz
	RCC->CFGR|=(4U<<10);            // APB1 prescaler = 2, 84MHz/2 = 42MHz (max)
	RCC->CFGR&=~(7U<<13);           // APB2 prescaler = 1, 84MHz

	// Select PLL as system clock
	RCC->CFGR |= (2 << 0);          // SW = PLL
	while((RCC->CFGR & (3 << 2)) != (2 << 2))  // Wait until PLL is system clock
		;
	
	// Update system core clock variable
	SystemCoreClockUpdate();
}

void gpioConfig()
{
	RCC->AHB1ENR|=(1<<0);           // GPIOA clock enable
	
	// Configure PA2, PA3 for UART2 (Alternate Function)
	GPIOA->MODER|=(2U<<4)|(2U<<6);  // PA2, PA3 as alternate function
	GPIOA->OSPEEDR |= (3 << 4) | (3 << 6);  // Very high speed
	GPIOA->AFR[0] |= (7U << 8) | (7U << 12); // AF7 (USART2)
	
	// Configure PA5 for LED (Output)
	GPIOA->MODER|=(1<<10);          // PA5 as output
	GPIOA->PUPDR &=~(3U<<10);       // No pull-up/pull-down
}

void turnLed()
{
	GPIOA->ODR |= (1<<5);           // Set PA5 high
}

void turnOffLed()
{
	GPIOA->ODR &= ~(1<<5);          // Set PA5 low
}

void uartConfig()
{
	RCC->APB1ENR|=(1<<17);          // USART2 clock enable
	
	USART2->CR1&=~(1<<13);          // USART2 disable
	
	// Baud rate calculation: BRR = fPCLK / (16 × Baud Rate)
	// BRR = 42,000,000 / (16 × 115,200) = 22.786
	// Mantissa = 22, Fraction = 0.786 × 16 ˜ 13
	uint32_t mantissa = 22;         // Integer part
	uint32_t fraction = 13;         // Fractional part (0-15)
	USART2->BRR = (mantissa << 4) | (fraction & 0x0F);
	
	// 8N1 format: 8 data bits, no parity, 1 stop bit
	USART2->CR1 &= ~((1 << 12) | (1 << 10));  // 8 data bits, parity disabled
	USART2->CR2 &= ~(3 << 12);      // 1 stop bit
	
	USART2->CR1 |= (1 << 3);        // TX enable
	USART2->CR1 |= (1 << 2);        // RX enable
	
	USART2->CR1|= (1<<13);          // UART enable
}

void UART2_SendChar(char ch)
{
    while(!(USART2->SR & (1 << 7)));  // Wait for TXE flag
    USART2->DR = ch;
}

void UART2_SendString(char *str)
{
    while(*str)
    {
        UART2_SendChar(*str);
        str++;
    }
}

// Read single character
uint8_t UART2_ReceiveChar(void)
{
    while(!(USART2->SR & (1 << 5)));  // Wait for RXNE (Receive Not Empty) flag
    uint8_t temp = USART2->DR;        // Read 8-bit data
    return temp;
}

// Check if data is available
int UART2_DataAvailable(void)
{
    return (USART2->SR & (1 << 5)) ? 1 : 0;
}

void UART2_ReceiveString(char *buffer, uint16_t maxLen)
{
    uint16_t i=0;
    char ch;

    while(i < maxLen - 1)
    {
        ch = UART2_ReceiveChar();
        
        // Stop on Enter (CR or LF)
        if(ch == '\r' || ch == '\n')
        {
            break;
        }
        
        // Handle backspace
        if(ch == '\b' && i > 0) 
        {
            i--;                        
            UART2_SendString("\b \b");  
            continue;                   
        }
        
        // Accept only printable ASCII characters
        if(ch>=32 && ch<=126)
        {
            buffer[i++]=ch;
            UART2_SendChar(ch);         // Echo character
        }
    }
    buffer[i] = '\0';                   
}

void delay(uint32_t time)
{
    while(time--);
}