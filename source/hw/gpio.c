// -----------------------------------------------------------------------------
// gpio.c - MPF - Nov 19, 2011
// -----------------------------------------------------------------------------

// includes
#include "stm32f10x.h"
#include "../stdint.h"
#include "gpio.h"

// gpio select list - NOTE: MUST BE ORDERED EXACTLY AS ENUM IN .h FILE
const struct gpioSelect_struct {
	GPIO_TypeDef * port;
	uint16_t pin;
} gpioSelect[GPIO_COUNT] = {
	GPIOA, GPIO_Pin_11,  // GPIO_PA11
	GPIOA, GPIO_Pin_12,  // GPIO_PA12
	GPIOB, GPIO_Pin_5,   // GPIO_PB5
	GPIOB, GPIO_Pin_8,   // GPIO_PB8
	GPIOB, GPIO_Pin_9,   // GPIO_PB9
	GPIOC, GPIO_Pin_6,   // GPIO_PC6
	GPIOC, GPIO_Pin_7,   // GPIO_PC7
	GPIOC, GPIO_Pin_8,   // GPIO_PC8
};

// module structure

// prototypes

// -----------------------------------------------------------------------------
// gpio initialization
// -----------------------------------------------------------------------------
void GPIO_Initialize(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	
	// enable Clocks to GPIO
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	// GPIO A ------------------------------------------------------------------
	// output configuration: A11, A12 as outputs, default to low
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// input configuration: NONE
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// GPIO B ------------------------------------------------------------------
	// output configuration: B5, B8, B9 as outputs, default to low
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// input configuration: NONE
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// GPIO C ------------------------------------------------------------------
	// output configuration: NONE
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = 0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// input configuration: PC6, PC7, PC8 as inputs
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

// -----------------------------------------------------------------------------
// set gpio
// -----------------------------------------------------------------------------
void GPIO_Set(int io){
	if (io >= GPIO_COUNT) return;
	GPIO_WriteBit(gpioSelect[io].port, gpioSelect[io].pin, Bit_SET);
}

// -----------------------------------------------------------------------------
// reset gpio
// -----------------------------------------------------------------------------
void GPIO_Reset(int io){
	if (io >= GPIO_COUNT) return;
	GPIO_WriteBit(gpioSelect[io].port, gpioSelect[io].pin, Bit_RESET);
}

// -----------------------------------------------------------------------------
// toggle GPIO
// -----------------------------------------------------------------------------
void GPIO_Toggle(int io){
	if (io >= GPIO_COUNT) return;
	if(GPIO_ReadOutputDataBit(gpioSelect[io].port, gpioSelect[io].pin) == Bit_RESET) {
		GPIO_WriteBit(gpioSelect[io].port, gpioSelect[io].pin, Bit_SET);
	} else {
		GPIO_WriteBit(gpioSelect[io].port, gpioSelect[io].pin, Bit_RESET);
	}
}

// -----------------------------------------------------------------------------
// read output bits
// -----------------------------------------------------------------------------
int GPIO_ReadOutput(int io){
	if (io >= GPIO_COUNT) return 0;
	if(GPIO_ReadOutputDataBit(gpioSelect[io].port, gpioSelect[io].pin) == Bit_SET) return 1;
	else return 0;
}

// -----------------------------------------------------------------------------
// read input bits
// -----------------------------------------------------------------------------
int GPIO_ReadInput(int io){
	// returns 1 if set, 0 if reset
	if (io >= GPIO_COUNT) return 0;
	if(GPIO_ReadInputDataBit(gpioSelect[io].port, gpioSelect[io].pin) == Bit_SET) return 1;
	else return 0;

}


