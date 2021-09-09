// -----------------------------------------------------------------------------
// PWM - MPF 08/2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "lib.h"
#include "dpwm.h"
#include "comm.h"

// module structure
struct dpwm_struct {
	int mode;
	struct lib_abcqd0struct pwm;
} dpwm;

// -----------------------------------------------------------------------------
// initialize module
// -----------------------------------------------------------------------------
void DPWM_Initialize(void){
	// default mode
	dpwm.mode = DPWM_SPWM;
	// initialize variables
	dpwm.pwm.q = dpwm.pwm.z = 0;
}

// -----------------------------------------------------------------------------
// set mode
// -----------------------------------------------------------------------------
void DPWM_SetMode(int mode){
	if (mode < DPWM_COUNT) dpwm.mode = mode;
}

// -----------------------------------------------------------------------------
// set pwm - note: it does not check for saturation: |mag(q,d)|<=1
// -----------------------------------------------------------------------------
void DPWM_Set(int16_t qvalue, int16_t dvalue, int16_t angle){
	int16_t val1, val2, val3;

	// if mode is other than SPWM, increment amplitude by 15%
	if (dpwm.mode != DPWM_SPWM) {
		qvalue = ((int32_t) qvalue * 18918) >> 14;
		dvalue = ((int32_t) dvalue * 18918) >> 14;
	}

	// generate PWM abc components
	dpwm.pwm.q = qvalue;
	dpwm.pwm.d = dvalue;
	LIB_QD0toABC(&dpwm.pwm, angle);

	switch (dpwm.mode){
	case DPWM_SPWM: 	// sine pwm --------------------------------------------
		val1 = dpwm.pwm.a;
		val2 = dpwm.pwm.b;
		val3 = dpwm.pwm.c;
		break;
	case DPWM_THIPWM:	// sine + third harmonic -------------------------------
		{
			// XXX need new algorithm for THIPWM that does not use angle as input
			dpwm.mode = DPWM_SVPWM;
		}
//		int16_t temp;
//		val1 = dpwm.pwm.a;
//		val2 = dpwm.pwm.b;
//		val3 = dpwm.pwm.c;
//		// calculate third harmonic
//		if (angle > ANGLE_240DEG) angle -= ANGLE_240DEG;
//		if (angle > ANGLE_120DEG) angle -= ANGLE_120DEG;
//		angle *= 3;
//		temp = ((int32_t) (1<<FP_AMPLITUDE) * 21845)>>17;	// 1/6
//		temp = ((int32_t) temp * LIB_SinAngle(angle))>>FP_AMPLITUDE;
//		// add third harmonic to values
//		val1 += temp;
//		val2 += temp;
//		val3 += temp;
//		}
		break;
	case DPWM_SVPWM:	// space vector (max min) ------------------------------
		{
		int16_t max, min, temp;
		// min-max method for svpwm
		max = min = dpwm.pwm.a;
		if (dpwm.pwm.b > max) max = dpwm.pwm.b;
		else if (dpwm.pwm.b < min) min = dpwm.pwm.b;
		if (dpwm.pwm.c > max) max = dpwm.pwm.c;
		else if (dpwm.pwm.c < min) min = dpwm.pwm.c;
		temp = (max+min)/2;
		// set values - offset
		val1 = dpwm.pwm.a - temp;
		val2 = dpwm.pwm.b - temp;
		val3 = dpwm.pwm.c - temp;
		}
		break;
	case DPWM_DPWMMIN:	// dpwm min --------------------------------------------
		{
		int16_t min, temp;
		// dpwm min
		min = dpwm.pwm.a;
		if (dpwm.pwm.b < min) min = dpwm.pwm.b;
		if (dpwm.pwm.c < min) min = dpwm.pwm.c;
		// find value to subtract to all pwms
		temp = min + (1<<FP_AMPLITUDE);
		// subtract to all values
		val1 = dpwm.pwm.a - temp;
		val2 = dpwm.pwm.b - temp;
		val3 = dpwm.pwm.c - temp;
		}
		break;
	case DPWM_DPWM1:	// dpwm1 -----------------------------------------------
		{
		int16_t max, value;
		// find maximum
		max = LIB_Abs16(dpwm.pwm.a);
		value = dpwm.pwm.a;
		if (LIB_Abs16(dpwm.pwm.b)>max) {
			max = LIB_Abs16(dpwm.pwm.b);
			value = dpwm.pwm.b;
		}
		if (LIB_Abs16(dpwm.pwm.c)>max) {
			max = LIB_Abs16(dpwm.pwm.c);
			value = dpwm.pwm.c;
		}
		// find amount to offset (add)
		if (value>0) value = (1<<FP_AMPLITUDE) - max;
		else value = -(1<<FP_AMPLITUDE) + max;
		// add to all phases
		val1 = dpwm.pwm.a + value;
		val2 = dpwm.pwm.b + value;
		val3 = dpwm.pwm.c + value;
		}
		break;
	}

	// save values in pwm structure to be retrieved by PWM_Get
	dpwm.pwm.a = val1;
	dpwm.pwm.b = val2;
	dpwm.pwm.c = val3;

	// convert from +1/-1 to 0-1
	val1 = ((1<<FP_AMPLITUDE)/2) + (val1/2);
	val2 = ((1<<FP_AMPLITUDE)/2) + (val2/2);
	val3 = ((1<<FP_AMPLITUDE)/2) + (val3/2);

	// write to PWM module
	PWM_Set(val1, val2, val3);
}

// -----------------------------------------------------------------------------
// get values of PWM - pwm ranges from -1 to 1
// -----------------------------------------------------------------------------
int16_t DPWM_Get(int phase){
	if (phase == DPWM_PHASE_A) return dpwm.pwm.a;
	if (phase == DPWM_PHASE_B) return dpwm.pwm.b;
	if (phase == DPWM_PHASE_C) return dpwm.pwm.c;
	// else
	return 0;
}

// -----------------------------------------------------------------------------
// get phase with maximum pwm value - used to reconstruct three phase currents
// -----------------------------------------------------------------------------
int	DPWM_GetPhaseWithMaxPWM(void){
	int16_t max;
	int maxPhase = DPWM_PHASE_A;
	max = dpwm.pwm.a;
	if (dpwm.pwm.b > max) {
		max = dpwm.pwm.b;
		maxPhase = DPWM_PHASE_B;
	}
	if (dpwm.pwm.c > max) {
		maxPhase = DPWM_PHASE_C;
	}
	return maxPhase;
}
