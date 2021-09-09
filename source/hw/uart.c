// -----------------------------------------------------------------------------
// UART - MPF 08/2011
// -----------------------------------------------------------------------------

// includes 
#include "stm32f10x.h"
#include "../stdint.h"
#include "uart.h"

// -----------------------------------------------------------------------------
// initialize UART
// -----------------------------------------------------------------------------
void UART_Init(int32_t baudRate){

	USART_InitTypeDef UART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIOB, USART1 and AFIO clocks 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);

	// Remap UART1 to PB6 / PB7 
	GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
	
	// Configure UART1 Tx as alternate function push-pull 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// Configure UART1 Rx as input floating 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// UART1 configuration 
	UART_InitStructure.USART_BaudRate = baudRate;
	UART_InitStructure.USART_WordLength = USART_WordLength_8b;
	UART_InitStructure.USART_StopBits = USART_StopBits_1;
	UART_InitStructure.USART_Parity = USART_Parity_No ;
	UART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &UART_InitStructure);

	// Enable the UART1 
	USART_Cmd(USART1, ENABLE);
 
}

// -----------------------------------------------------------------------------
// write byte
// -----------------------------------------------------------------------------
void UART_WriteByte(uint16_t data){
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, data);
}

// -----------------------------------------------------------------------------
// write word, MSB first
// -----------------------------------------------------------------------------
void UART_WriteWord(uint16_t data){
	UART_WriteByte((data >> 8) & 0x00FF);
	UART_WriteByte(data & 0x00FF);
}

// -----------------------------------------------------------------------------
// read byte
// -----------------------------------------------------------------------------
uint16_t UART_ReadByte(void){
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET) {}
	return (USART_ReceiveData(USART1) & 0x00FF); 
}

// -----------------------------------------------------------------------------
// read word, MSB first
// -----------------------------------------------------------------------------
uint16_t UART_ReadWord(void){
	uint16_t data;
	data = (UART_ReadByte() & 0x00FF)<<8;						    
	data += UART_ReadByte() & 0x00FF;       
    return data;    
 }


