// -----------------------------------------------------------------------------
// ADC - MPF 08/2011
// -----------------------------------------------------------------------------

// include files
#include "stm32f10x.h"
#include "../stdint.h"
#include "adc.h"
#include "../globals.h"

// definitions
#define ADC_NR_CHANNELS      7   // number of channels to scan
#define JADC_NR_CHANNELS     1   // number of injected channels to be scanned
#define ADC1_DR_Address    ((u32)0x4001244C)   	// address of ADC data register
#define DMA_BUFFER_SIZE (ADC_NR_CHANNELS)


// ADC1 channels are scanned in the following order:
// COMMENT UNUSED CHANNELS IN INITIALIZATION
#define ADC_1   ADC_Channel_8		// VBUSFB
#define ADC_2 	ADC_Channel_9		// HSTFB
#define ADC_3   ADC_Channel_5		// VAFB
#define ADC_4   ADC_Channel_6		// VBFB
#define ADC_5   ADC_Channel_7		// VCFB
#define ADC_6   ADC_Channel_10		// PHA
#define ADC_7   ADC_Channel_11		// PHB
//#define ADC_8   ADC_Channel_9		// NOT USED

// Injected ADC1 channels are scanned in the following order:
// COMMENT UNUSED CHANNELS IN INITIALIZATION
#define JADC1_1  	ADC_Channel_3		// IBFB
//#define JADC1_2 	ADC_Channel_5		// *** NOT USED ***
//#define JADC1_3 	ADC_Channel_6		// *** NOT USED ***
//#define JADC1_4 	ADC_Channel_7		// *** NOT USED ***

// Injected ADC2 channels are scanned in the following order:
// COMMENT UNUSED CHANNELS IN INITIALIZATION
#define JADC2_1  	ADC_Channel_4		// ICFB
//#define JADC2_2 	ADC_Channel_4		// *** NOT USED ***
//#define JADC2_3 	ADC_Channel_4		// *** NOT USED ***
//#define JADC2_4 	ADC_Channel_4		// *** NOT USED ***

// Injected ADC3 channels are scanned in the following order:
// COMMENT UNUSED CHANNELS IN INITIALIZATION
// ADC3 IS NOT MAPPED TO SAME PINS AS ADC1 AND ADC2! Watch for mapping issues
#define JADC3_1  	ADC_Channel_2		// IAFB
//#define JADC3_2 	ADC_Channel_2		// *** NOT USED ***
//#define JADC3_3 	ADC_Channel_2		// *** NOT USED ***
//#define JADC3_4 	ADC_Channel_2		// *** NOT USED ***

// module structures
struct adc_struct { 
	volatile int16_t channel[ADC_NR_CHANNELS];
} adc;

// -----------------------------------------------------------------------------
// initialize module
// -----------------------------------------------------------------------------
void ADC_Initialize(void){

	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;	

	// Clock configuration -----------------------------------------------------
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2 |
						   RCC_APB2Periph_ADC3, ENABLE);
	
	// GPIO configuration ------------------------------------------------------
	// Configure GPIOA analog inputs: A0 to A7
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
								  GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
								  GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure GPIOB analog inputs: B0 to B1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// Configure GPIOC analog inputs: C0 to C2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// DMA1 channel1 configuration ---------------------------------------------
	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&adc;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = DMA_BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	
	// Enable DMA1 channel1 
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	// ADC1 configuration ------------------------------------------------------
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T4_CC4;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = ADC_NR_CHANNELS;
	ADC_Init(ADC1, &ADC_InitStructure);

	// ADC2 configuration ------------------------------------------------------
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_Mode = ADC_Mode_InjecSimult;
	ADC_Init(ADC2, &ADC_InitStructure);

	// ADC3 configuration ------------------------------------------------------
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_Mode = ADC_Mode_InjecSimult;
	ADC_Init(ADC3, &ADC_InitStructure);


	// ADC1 regular configuration ----------------------------------------------
	// conversion time: (sample time + 12.5 cycles) x ADC_CLOCK (12Mhz) 
	// COMMENT CONFIGS NOT NEEDED!
	ADC_RegularChannelConfig(ADC1, ADC_1, 1, ADC_SampleTime_7Cycles5);  
	ADC_RegularChannelConfig(ADC1, ADC_2, 2, ADC_SampleTime_7Cycles5);  
	ADC_RegularChannelConfig(ADC1, ADC_3, 3, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_4, 4, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_5, 5, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_6, 6, ADC_SampleTime_7Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_7, 7, ADC_SampleTime_7Cycles5);
	//ADC_RegularChannelConfig(ADC1, ADC_8, 8, ADC_SampleTime_7Cycles5);

	// Enable ADC1 DMA
	ADC_DMACmd(ADC1, ENABLE);
	
	// Enable ADC1 external trigger
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);

	// JADC1 configuration -----------------------------------------------------
	// Set injected sequencer length 
	ADC_InjectedSequencerLengthConfig(ADC1, JADC_NR_CHANNELS);

	// JADC1 injected channel configuration  - COMMENT CONFIGS NOT NEEDED!
	ADC_InjectedChannelConfig(ADC1, JADC1_1, 1, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC1, JADC1_2, 2, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC1, JADC1_3, 3, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC1, JADC1_4, 4, ADC_SampleTime_7Cycles5);

	// JADC1 injected external trigger configuration 
	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO);
	
	// Enable JADC1 injected external trigger conversion 
	ADC_ExternalTrigInjectedConvCmd(ADC1, ENABLE);
	
	// JADC2 configuration -----------------------------------------------------
	// Set injected sequencer length
	ADC_InjectedSequencerLengthConfig(ADC2, JADC_NR_CHANNELS);

	// JADC2 injected channel configuration  - COMMENT CONFIGS NOT NEEDED!
	ADC_InjectedChannelConfig(ADC2, JADC2_1, 1, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC2, JADC2_2, 2, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC2, JADC2_3, 3, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC2, JADC2_4, 4, ADC_SampleTime_7Cycles5);

	// JADC2 injected external trigger configuration
	ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_T1_TRGO);

	// Enable JADC2 injected external trigger conversion
	ADC_ExternalTrigInjectedConvCmd(ADC2, ENABLE);

	// JADC3 configuration -----------------------------------------------------
	// Set injected sequencer length
	ADC_InjectedSequencerLengthConfig(ADC3, JADC_NR_CHANNELS);

	// JADC3 injected channel configuration  - COMMENT CONFIGS NOT NEEDED!
	ADC_InjectedChannelConfig(ADC3, JADC3_1, 1, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC3, JADC3_2, 2, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC3, JADC3_3, 3, ADC_SampleTime_7Cycles5);
	//ADC_InjectedChannelConfig(ADC3, JADC3_4, 4, ADC_SampleTime_7Cycles5);

	// JADC3 injected external trigger configuration
	ADC_ExternalTrigInjectedConvConfig(ADC3, ADC_ExternalTrigInjecConv_T1_TRGO);

	// Enable JADC3 injected external trigger conversion
	ADC_ExternalTrigInjectedConvCmd(ADC3, ENABLE);

	// Enable ADC --------------------------------------------------------------

	// Enable ADC - THIS NEEDS TO BE LAST COMMAND BEFORE CALIBRATION AND AFTER
	// ALL CONFIGURATION IS FINALIZED
	ADC_Cmd(ADC1, ENABLE);
	ADC_Cmd(ADC2, ENABLE);
	ADC_Cmd(ADC3, ENABLE);

	// ADC1 calibration --------------------------------------------------------
	// Enable ADC1 reset calibaration register    
	ADC_ResetCalibration(ADC1);
	// Check the end of ADC1 reset calibration register 
	while(ADC_GetResetCalibrationStatus(ADC1));
	// Start ADC1 calibration 
	ADC_StartCalibration(ADC1);
	// Check the end of ADC1 calibration 
	while(ADC_GetCalibrationStatus(ADC1));  

	// ADC2 calibration --------------------------------------------------------
	// Enable ADC2 reset calibaration register
	ADC_ResetCalibration(ADC2);
	// Check the end of ADC1 reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC2));
	// Start ADC1 calibration
	ADC_StartCalibration(ADC2);
	// Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC2));

	// ADC3 calibration --------------------------------------------------------
	// Enable ADC3 reset calibaration register
	ADC_ResetCalibration(ADC3);
	// Check the end of ADC1 reset calibration register
	while(ADC_GetResetCalibrationStatus(ADC3));
	// Start ADC1 calibration
	ADC_StartCalibration(ADC3);
	// Check the end of ADC1 calibration
	while(ADC_GetCalibrationStatus(ADC3));
}

// -----------------------------------------------------------------------------
// ADC Read, channel 0 to (ADC_NR_CHANNELS-1)
// -----------------------------------------------------------------------------
 int16_t ADC_Read(int channel){
	return adc.channel[channel];
}

// -----------------------------------------------------------------------------
// JADC1 Read, channel 0 to (JADC_NR_CHANNELS-1)
// -----------------------------------------------------------------------------
int16_t JADC1_Read(int channel){
	int16_t value;
	switch (channel){
		case 0:
		value = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
		break;
		case 1:
		value = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
		break;
		case 2:
		value = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_3);
		break;
		case 3:
		value = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_4);
		break;
		default:
		value = 0;
		break;
	}
	return value;
}

// -----------------------------------------------------------------------------
// JADC2 Read, channel 0 to (JADC_NR_CHANNELS-1)
// -----------------------------------------------------------------------------
int16_t JADC2_Read(int channel){
	int16_t value;
	switch (channel){
		case 0:
		value = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_1);
		break;
		case 1:
		value = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_2);
		break;
		case 2:
		value = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_3);
		break;
		case 3:
		value = ADC_GetInjectedConversionValue(ADC2, ADC_InjectedChannel_4);
		break;
		default:
		value = 0;
		break;
	}
	return value;
}

// -----------------------------------------------------------------------------
// JADC3 Read, channel 0 to (JADC_NR_CHANNELS-1)
// -----------------------------------------------------------------------------
int16_t JADC3_Read(int channel){
	int16_t value;
	switch (channel){
		case 0:
		value = ADC_GetInjectedConversionValue(ADC3, ADC_InjectedChannel_1);
		break;
		case 1:
		value = ADC_GetInjectedConversionValue(ADC3, ADC_InjectedChannel_2);
		break;
		case 2:
		value = ADC_GetInjectedConversionValue(ADC3, ADC_InjectedChannel_3);
		break;
		case 3:
		value = ADC_GetInjectedConversionValue(ADC3, ADC_InjectedChannel_4);
		break;
		default:
		value = 0;
		break;
	}
	return value;
}

// -----------------------------------------------------------------------------
// JADC Enable IRQ
// -----------------------------------------------------------------------------
void JADC_EnableIRQ(void){
	ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
}

// -----------------------------------------------------------------------------
// ADC Enable IRQ
// -----------------------------------------------------------------------------
void ADC_EnableIRQ(void){
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
}


