//------------------------------------------------------------------------------
// Feedbacks - MPF 08/2011
//------------------------------------------------------------------------------

// include files
#include "../stdint.h"
#include "../globals.h"
#include "../app/lib.h"
#include "fb.h"
#include "fb_tables.h"
#include "../hw/adc.h"
#include "../hw/tmr.h"

// defines
#define NR_FAST_CHANNELS      3   // number of fast channels (injected)
#define NR_SLOW_CHANNELS      7   // number of slow channels (regular)
#define AVG_SAMPLES		      3	  // 2^n number of samples to average

// feedback data structure
struct fbdata_struct {
	int16_t raw;
	int16_t rawBuffer[1<<AVG_SAMPLES];	// for averaging
	int32_t rawSum;					// for averaging
	int16_t rawOffset;
	int16_t scaling;
	int16_t scalingFp;
	int16_t result;
	int16_t average;					// average of result
	int16_t filter;					// digital filter of result
} fbdata[NR_FAST_CHANNELS+NR_SLOW_CHANNELS];

// module structure
struct fb_struct {
	int fastIndex;
	int slowIndex;
} fb;

//------------------------------------------------------------------------------
// module initialize
//------------------------------------------------------------------------------
void FB_Initialize(void){
	int i, j;
	
	// initialize required modules
	ADC_Initialize();
	TMR4_Init();
	
	// zero all variables
	for (i=0; i < NR_FAST_CHANNELS+NR_SLOW_CHANNELS; i++){
		fbdata[i].raw = 0;						
		fbdata[i].rawOffset = 0;
		for (j=0; j < (1<<AVG_SAMPLES); j++) fbdata[i].rawBuffer[j] = 0;	
		fbdata[i].rawSum = 0;						
		fbdata[i].scaling = 0;					
		fbdata[i].scalingFp = 0;						
		fbdata[i].result = 0;
		fbdata[i].average = 0;								
		fbdata[i].filter = 0;			
	}	
	fb.fastIndex = 0;
	fb.slowIndex = 0;

	// initialize offsets and scaling
	
	// current shunts:
	// 25m ohm shunt, 3.92 Gain amplifier, to FP10 currents
	// scaling factor = 1 / 4096 * 3.3 / 3.92 / 0.025 * 2^10
	// negative sign: current measured is positive going OUT of the phase leg
	fbdata[FB_IA].rawOffset  = 0;
	fbdata[FB_IA].scaling    = -17240;
	fbdata[FB_IA].scalingFp  = 11;
	                        
	fbdata[FB_IB].rawOffset  = 0;
	fbdata[FB_IB].scaling    = -17240;
	fbdata[FB_IB].scalingFp  = 11;
	                        
	fbdata[FB_IC].rawOffset  = 0;
	fbdata[FB_IC].scaling    = -17240;
	fbdata[FB_IC].scalingFp  = 11;
	
	// back emf voltage:
	// divider 2x470k & 6.8k, gain = 0.007182, to FP6 voltage
	// scaling factor: 1 / 4096 * 3.3 / 0.007182 * 2^6
	fbdata[FB_VA].rawOffset  = 0;
	fbdata[FB_VA].scaling    = 29406;
	fbdata[FB_VA].scalingFp  = 12;

	fbdata[FB_VB].rawOffset  = 0;
	fbdata[FB_VB].scaling    = 29406;
	fbdata[FB_VB].scalingFp  = 12;

	fbdata[FB_VC].rawOffset  = 0;
	fbdata[FB_VC].scaling    = 29406;
	fbdata[FB_VC].scalingFp  = 12;

	// bus voltage:
	// divider 2x470k & 6.8k, gain = 0.007182, to FP6 voltage
	// scaling factor: 1 / 4096 * 3.3 / 0.007182 * 2^6
	fbdata[FB_VBUS].rawOffset  = 0;
	fbdata[FB_VBUS].scaling    = 29406;
	fbdata[FB_VBUS].scalingFp  = 12;

	// heatsink temperature:
	// using table lookup from 3.3V ADC Counts to Temp FP7
	fbdata[FB_HST].rawOffset   = 0;
	fbdata[FB_HST].scaling     = 1;
	fbdata[FB_HST].scalingFp   = 0;

	// sin/cos encoder ph A:
	fbdata[FB_PHA].rawOffset   = 1635;
	fbdata[FB_PHA].scaling     = 1;
	fbdata[FB_PHA].scalingFp   = 0;

	// sin/cos encoder ph B:
	fbdata[FB_PHB].rawOffset   = 1601;
	fbdata[FB_PHB].scaling     = 24207;
	fbdata[FB_PHB].scalingFp   = 14;
}

//------------------------------------------------------------------------------
// FB read from JADC - call from JADC EOC	
//------------------------------------------------------------------------------
void FB_ReadFastADC(void){

	int i;

	// read data from ADC
	fbdata[FB_IA].raw  = JADC3_Read(0);
	fbdata[FB_IB].raw  = JADC1_Read(0);
	fbdata[FB_IC].raw  = JADC2_Read(0);
	
	// process data
	for (i=0; i<NR_FAST_CHANNELS; i++){
		// add new item to buffer
		fbdata[i].rawSum -= fbdata[i].rawBuffer[fb.fastIndex];
		fbdata[i].rawBuffer[fb.fastIndex] = fbdata[i].raw;
		fbdata[i].rawSum += fbdata[i].raw;
		// calculate instantaneous result
		fbdata[i].result = fbdata[i].raw - fbdata[i].rawOffset;
		fbdata[i].result = ((int32_t) fbdata[i].result * fbdata[i].scaling)>>fbdata[i].scalingFp;
		// calculate average result
		fbdata[i].average = fbdata[i].rawSum >> AVG_SAMPLES;
		fbdata[i].average -= fbdata[i].rawOffset;
		fbdata[i].average = ((int32_t) fbdata[i].average * fbdata[i].scaling)>>fbdata[i].scalingFp;
	}
	
	// increment index, wrap
	fb.fastIndex++;
	if (fb.fastIndex >= (1<<AVG_SAMPLES)) fb.fastIndex = 0;
		
}

//------------------------------------------------------------------------------
// FB read from ADC - call from DMA EOT 
//------------------------------------------------------------------------------
void FB_ReadSlowADC(void){
	int i;
	
	// read data from ADC
	fbdata[FB_VBUS].raw  = ADC_Read(0);
	fbdata[FB_HST].raw   = LIB_TableLookUp(ADC_Read(1), hstherm_table, HSTHERM_TABLE_SIZE);
	fbdata[FB_VA].raw    = ADC_Read(2);
	fbdata[FB_VB].raw    = ADC_Read(3);
	fbdata[FB_VC].raw    = ADC_Read(4);
	fbdata[FB_PHA].raw    = ADC_Read(5);
	fbdata[FB_PHB].raw    = ADC_Read(6);
	
	// process data
	for (i=NR_FAST_CHANNELS; i<(NR_FAST_CHANNELS+NR_SLOW_CHANNELS); i++){
		// add new item to buffer
		fbdata[i].rawSum -= fbdata[i].rawBuffer[fb.slowIndex];
		fbdata[i].rawBuffer[fb.slowIndex] = fbdata[i].raw;
		fbdata[i].rawSum += fbdata[i].raw;
		// calculate instantaneous result
		fbdata[i].result = fbdata[i].raw - fbdata[i].rawOffset;
		fbdata[i].result = ((int32_t) fbdata[i].result * fbdata[i].scaling)>>fbdata[i].scalingFp;
		// calculate average result
		fbdata[i].average = fbdata[i].rawSum >> AVG_SAMPLES;
		fbdata[i].average -= fbdata[i].rawOffset;
		fbdata[i].average = ((int32_t) fbdata[i].average * fbdata[i].scaling)>>fbdata[i].scalingFp;
	}	

	// increment index, wrap
	fb.slowIndex++;
	if (fb.slowIndex >= (1<<AVG_SAMPLES)) fb.slowIndex = 0;
}

//------------------------------------------------------------------------------
// get feedback
//------------------------------------------------------------------------------
int16_t FB_Get(int index){
	int16_t value;

	if (index >= FB_COUNT) value = 0;
	else value = fbdata[index].result;
	return value;
}

//------------------------------------------------------------------------------
// get average
//------------------------------------------------------------------------------
int16_t FB_GetAverage(int index){
	int16_t value;

	if (index >= FB_COUNT) value = 0;
	else value = fbdata[index].average;
	return value;
}

//------------------------------------------------------------------------------
// get filter
//------------------------------------------------------------------------------
int16_t FB_GetFilter(int index){
	int16_t value;

	if (index >= FB_COUNT) value = 0;
	else value = fbdata[index].filter;
	return value;
}

//------------------------------------------------------------------------------
// initialize offsets
//------------------------------------------------------------------------------
void FB_InitOffsets(void){

	fbdata[FB_IA].rawOffset    = fbdata[FB_IA].rawSum >> AVG_SAMPLES;
	fbdata[FB_IB].rawOffset    = fbdata[FB_IB].rawSum >> AVG_SAMPLES;
	fbdata[FB_IC].rawOffset    = fbdata[FB_IC].rawSum >> AVG_SAMPLES;
}





