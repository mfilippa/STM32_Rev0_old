// -----------------------------------------------------------------------------
// Timers - MPF 08/2011
// -----------------------------------------------------------------------------

// includes 
#include "stm32f10x.h"
#include "../stdint.h"
#include "tmr.h"
#include "../globals.h"

// hardware configuration
#define LOW_PWM_INVERTED		// uncomment if low side PWM is inverted
#define BREAK_PIN_ENABLED		// uncomment to enable break pin

// TMR1 definitions	
#define TMR1_DUTYCYCLE    (TMR1_PERIOD/2)    	// 50%
#define TMR1_DEADTIME      72        			// 1us deadtime
#define TMR1_CC4_PULSE    (TMR1_PERIOD/2)       // IRQ trigger

// TMR2 definitions	
#define TMR2_DUTYCYCLE       0        // 0%

// TMR4 definitions	- tied to TIM1
#define TMR4_OC1_PULSE      1         // 
#define TMR4_OC4_PULSE      1         // ADC trigger
 
// -----------------------------------------------------------------------------
// initialize TMR1
// -----------------------------------------------------------------------------
void TMR1_Init(void){

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure;
	
	// TIM1, GPIOA and GPIOB clock enable 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA |  
	                     RCC_APB2Periph_GPIOB, ENABLE);

	// GPIOA Configuration: TIM1 Channel 1, 2 & 3 as alternate function push-pull 
	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// GPIOB Configuration: TIM1 Channel 1N, 2N & 3N as alternate function push-pull 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// GPIOB Configuration: BKIN pin (FAULT)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// TIM1 time Base configuration 
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
	TIM_TimeBaseStructure.TIM_Period = (TMR1_PERIOD - 1);
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 3;
 	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	// Channel 1, 2 & 3 Configuration in PWM mode 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
#ifdef LOW_PWM_INVERTED
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;		// inverted!
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;		// inverted!
#else
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
#endif
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;

	TIM_OCInitStructure.TIM_Pulse = TMR1_DUTYCYCLE;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  
    TIM_OC2Init(TIM1, &TIM_OCInitStructure); 
	TIM_OC3Init(TIM1, &TIM_OCInitStructure); 

	// OC Channel 4 configuration
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = TMR1_CC4_PULSE;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);

    // Automatic Output, Break, dead time and lock configuration
	TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_1;
	TIM_BDTRInitStructure.TIM_DeadTime = TMR1_DEADTIME;
#ifdef BREAK_PIN_ENABLED
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Enable;
#else
	TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
#endif
	TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
	TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;	
	TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

	// Set TIM1 trigger output TRGO 
	TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);

	// Enable preload for TIM1 
    TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);
	
	// set timer counter - needed to make update happen at the right place
	TIM_SetCounter(TIM1, (TMR1_PERIOD-1));

	// TIM1 counter enable 
	TIM_Cmd(TIM1, ENABLE);	

	// enable outputs
	TMR1_SetOutputState(DISABLE);
}

// -----------------------------------------------------------------------------
// enable TMR1 update interrupt
// -----------------------------------------------------------------------------
void TMR1_IRQUpdateEnable(void){
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE );	
}

// -----------------------------------------------------------------------------
// enable TMR1 CC4 interrupt
// -----------------------------------------------------------------------------
void TMR1_IRQCC4Enable(void){
	TIM_ITConfig(TIM1, TIM_IT_CC4, ENABLE );
}

// -----------------------------------------------------------------------------
// enable/disable BRK interrupt
// -----------------------------------------------------------------------------
void TMR1_SetBRKIRQState(int enable){
	// BRK interupt enable/disable
	if (enable == TMR_DISABLE) TIM_ITConfig(TIM1, TIM_IT_Break, DISABLE );
	else {
		TIM_ClearITPendingBit(TIM1, TIM_IT_Break);
		TIM_ITConfig(TIM1, TIM_IT_Break, ENABLE );
	}
}

// -----------------------------------------------------------------------------
// enable/disable TMR1 outputs
// -----------------------------------------------------------------------------
void TMR1_SetOutputState(int state){
	if (state == TMR_DISABLE) TIM_CtrlPWMOutputs(TIM1, DISABLE);
	else TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

// -----------------------------------------------------------------------------
// set TMR1 duty cycles
// -----------------------------------------------------------------------------
void TMR1_Set(uint16_t val1, uint16_t val2, uint16_t val3){
	if (val1 < TMR1_PERIOD) TIM_SetCompare1(TIM1, val1);
	if (val2 < TMR1_PERIOD) TIM_SetCompare2(TIM1, val2);
	if (val3 < TMR1_PERIOD) TIM_SetCompare3(TIM1, val3);
}

// -----------------------------------------------------------------------------
// disable TMR1 channel
// -----------------------------------------------------------------------------
void TMR1_DisableChannel(int channel){

switch (channel){
	case TMR1_NONE:
		TIM_CCxCmd(TIM1,  TIM_Channel_1, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1,  TIM_Channel_2, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1,  TIM_Channel_3, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
	break;
	case TMR1_CH1:
		TIM_CCxCmd(TIM1,  TIM_Channel_1, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Disable);
		TIM_CCxCmd(TIM1,  TIM_Channel_2, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1,  TIM_Channel_3, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
	break;
	case TMR1_CH2:
		TIM_CCxCmd(TIM1,  TIM_Channel_1, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1,  TIM_Channel_2, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable);
		TIM_CCxCmd(TIM1,  TIM_Channel_3, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCxN_Enable);
	break;
	case TMR1_CH3:
		TIM_CCxCmd(TIM1,  TIM_Channel_1, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_1, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1,  TIM_Channel_2, TIM_CCx_Enable);
		TIM_CCxNCmd(TIM1, TIM_Channel_2, TIM_CCxN_Enable);
		TIM_CCxCmd(TIM1,  TIM_Channel_3, TIM_CCx_Disable);
		TIM_CCxNCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable);
	break;
	}
}

// -----------------------------------------------------------------------------
// initialize TMR2
// -----------------------------------------------------------------------------
void TMR2_Init(void){

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	// TIM2, GPIOA clock enable 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    
	// GPIOA Configuration: TIM2 Channel 1, 2 as alternate function push-pull 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;    
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// TIM2 time Base configuration 
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = (TMR2_PERIOD-1);
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
 	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	// Channel 1, 2 Configuration in PWM mode 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

	TIM_OCInitStructure.TIM_Pulse = TMR2_DUTYCYCLE;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);   
  
	TIM_OCInitStructure.TIM_Pulse = TMR2_DUTYCYCLE;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);

	// Set TIM1 trigger output TRGO 
	TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

	// Enable preload for TIM2 
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);

	// TIM1 counter enable 
	TIM_Cmd(TIM2, ENABLE);	
}

// -----------------------------------------------------------------------------
// set TMR2 Channel 1
// -----------------------------------------------------------------------------
void TMR2_SetCh1(uint16_t value){
	if (value < TMR2_PERIOD) TIM_SetCompare1(TIM2, value);
}

// -----------------------------------------------------------------------------
// set TMR2 Channel 2
// -----------------------------------------------------------------------------
void TMR2_SetCh2(uint16_t value){
	if (value < TMR2_PERIOD) TIM_SetCompare2(TIM2, value);
}

// -----------------------------------------------------------------------------
// initialize TMR4
// -----------------------------------------------------------------------------
void TMR4_Init(void){

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	// TIM4 clock enable 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); 
	
	// TIM4 select ITR0 as input clock and set slave mode 
	TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_External1);
	TIM_SelectInputTrigger(TIM4, TIM_TS_ITR0);

	// Time Base configuration 
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_Period = (TMR4_PERIOD-1);
 	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	// OC Channel 1 configuration 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_Pulse = TMR4_OC1_PULSE;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure); 

	// OC Channel 4 configuration 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse = TMR4_OC4_PULSE;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);

	// TIM4 counter enable 
	TIM_Cmd(TIM4, ENABLE);
}							  

// -----------------------------------------------------------------------------
// enable TMR4 CC1
// -----------------------------------------------------------------------------
void TMR4_IRQCC1Enable(void){
	TIM_ITConfig(TIM4, TIM_IT_CC1, ENABLE);
}

// -----------------------------------------------------------------------------
// enable TMR4 CC4
// -----------------------------------------------------------------------------
void TMR4_IRQCC4Enable(void){
	TIM_ITConfig(TIM4, TIM_IT_CC4, ENABLE);
}



