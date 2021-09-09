// -----------------------------------------------------------------------------
// hwcfg.c - MPF - Nov 24, 2011
// -----------------------------------------------------------------------------

// includes
#include "stm32f10x.h"
#include "../stdint.h"
#include "hwcfg.h"

// definitions

// module structure

// prototypes

// -----------------------------------------------------------------------------
// disable all interrupts
// -----------------------------------------------------------------------------
void HWCFG_DisableInterrupts(void){
	NVIC_DisableIRQ(TIM1_BRK_IRQn);
	NVIC_DisableIRQ(TIM1_CC_IRQn);
	NVIC_DisableIRQ(TIM1_UP_IRQn);
	NVIC_DisableIRQ(TIM4_IRQn);
	NVIC_DisableIRQ(ADC1_2_IRQn);
	NVIC_DisableIRQ(DMA1_Channel1_IRQn);
	NVIC_DisableIRQ(SysTick_IRQn);
}

// -----------------------------------------------------------------------------
// enable all interrupts
// -----------------------------------------------------------------------------
void HWCFG_EnableInterrupts(void){
	NVIC_InitTypeDef NVIC_InitStructure;

	//Pre-emption priority: 0-15 for pre-emption priority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	// systick interrupt -------------------------------------------------------
    // clock source for systick = HCLK @ 72Mhz
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
	// configure systick = 72E6 / 1000 = 72k ticks = 1kHz = 1ms
	SysTick_Config(SystemCoreClock/1000);
	// set Systick priority
	NVIC_SetPriority(SysTick_IRQn,15);

	// Enable the TIM1 BRK Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_BRK_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the TIM1 CC Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the TIM1 UP Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Enable the TIM4 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Configure and enable ADC interrupt
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// Configure and enable DMA interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

// -----------------------------------------------------------------------------
// configure clocks
// -----------------------------------------------------------------------------
void HWCFG_ConfigureClocks(void){

	// all clocks but systick already configured in system_stm32f10x.c
	// systick configured at enable interrupts since its configuration enables systick interrupt
	// XXX RCC initialize clocks here, comment in assy file
}
