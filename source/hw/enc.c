// -----------------------------------------------------------------------------
// Encoder - MPF 08/2011
// -----------------------------------------------------------------------------

// includes
#include "stm32f10x.h"
#include "../stdint.h"
#include "enc.h"
#include "../globals.h"


// -----------------------------------------------------------------------------
// initialize module
// -----------------------------------------------------------------------------
void ENC_Initialize(int encoderPPR){

	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef TIM_InputCaptureInitStruct;

	// GPIOC clock enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	// TIM 3 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	// alternate function peripheral enable
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	// GPIOC Configuration: PC6, PC7, PC8 as inputs
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// remap PC6, PC7 and PC8 to TIM3
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);

	// TMR3 timebase initialization
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = (2*encoderPPR-1);
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	// TMR3 input capture intialization, signal A
	TIM_ICStructInit(&TIM_InputCaptureInitStruct);
	TIM_InputCaptureInitStruct.TIM_Channel = TIM_Channel_1;
	TIM_InputCaptureInitStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_InputCaptureInitStruct.TIM_ICPrescaler = TIM_ICSelection_DirectTI;
	TIM_InputCaptureInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_InputCaptureInitStruct.TIM_ICFilter = 0x00;
	TIM_ICInit(TIM3, &TIM_InputCaptureInitStruct);

	// TMR3 input capture initialization, signal B
	TIM_InputCaptureInitStruct.TIM_Channel = 2;
	TIM_ICInit(TIM3, &TIM_InputCaptureInitStruct);

	// TMR3 configure encoder interface
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI1, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);

	// enable timer
	TIM_Cmd(TIM3, ENABLE);

}

// -----------------------------------------------------------------------------
// Read counter - returns TWICE the value of PPR
// -----------------------------------------------------------------------------
uint16_t ENC_ReadCounter(void){
	return TIM_GetCounter(TIM3);
}

// -----------------------------------------------------------------------------
// Set encoder PPR
// -----------------------------------------------------------------------------
void ENC_SetPPR(int ppr){
	// set timer reload
	TIM3->ARR = ppr-1 ;
	// reset counter
	TIM3->CNT = 0;
}

// -----------------------------------------------------------------------------
// Reset encoder count
// -----------------------------------------------------------------------------
void ENC_ResetCounter(void){
	TIM3->CNT = 0;
}
