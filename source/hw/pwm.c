// -----------------------------------------------------------------------------
// PWM - MPF 08/2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "pwm.h"
#include "io.h"
#include "../hw/tmr.h"


// -----------------------------------------------------------------------------
// initialize module
// -----------------------------------------------------------------------------
void PWM_Initialize(void){
	TMR1_Init();
	TMR2_Init();
}

// -----------------------------------------------------------------------------
// set values - values are in 0-1 amplitude
// -----------------------------------------------------------------------------
void PWM_Set(int16_t val1, int16_t val2, int16_t val3){
	val1 = ((int32_t) val1 * (TMR1_PERIOD-1))>>FP_AMPLITUDE;
	val2 = ((int32_t) val2 * (TMR1_PERIOD-1))>>FP_AMPLITUDE;
	val3 = ((int32_t) val3 * (TMR1_PERIOD-1))>>FP_AMPLITUDE;
	TMR1_Set(val1, val2, val3);
}

// -----------------------------------------------------------------------------
// set output state - disables or enables all PWM channels
// -----------------------------------------------------------------------------
void PWM_SetOutputState(int state){
	if (state == 0) {
		TMR1_SetOutputState(state);
	} else {
		// for ST module: before enabling, check that gate driver is enabled
		// then enable PWM break interrupt, then enable PWMs
		if (IO_ReadOutput(IO_GATEDRIVE)!=0){
			TMR1_SetBRKIRQState(1);
			TMR1_SetOutputState(1);
		}
	}
}

// -----------------------------------------------------------------------------
// PWM disable channel
// -----------------------------------------------------------------------------
void PWM_DisableChannel(int channel){
	TMR1_DisableChannel(channel);
}

// -----------------------------------------------------------------------------
// enable or disable break interrupt - 1 to enable, 0 to disable
// -----------------------------------------------------------------------------
void PWM_SetBreakIRQEnableState(int state){
	TMR1_SetBRKIRQState(0);
}

