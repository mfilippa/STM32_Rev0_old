//------------------------------------------------------------------------------
// BLDC - MPF 08/2011
//------------------------------------------------------------------------------

// include files
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "lib.h"
#include "bldc.h"
#include "ctrl.h"
#include "bldc_tables.h"
#include "dpwm.h"
#include "comm.h"

// defines
#define MIN_CURRENT_THRESHOLD	(102)	// 100mA FP10

// module structure
struct bldc_struc {
	int16_t amplitude;
	int direction;
	int16_t setpoint;
	int16_t feedback;
	int16_t angle;
	int16_t lastAngle;
	int16_t advanceAngle;
	int currentIndex;	// current commutation index
	int lastIndex;	    // last commutation index
	int16_t limit;
	struct lib_pistruct iPI;
	struct lib_pistruct qPI;
	struct lib_pistruct dPI;
	struct lib_abcqd0struct phCurrent;
} bldc;

// prototypes
void BLDC_SetCommutation(void);

//------------------------------------------------------------------------------
// Initialize module
//------------------------------------------------------------------------------
void BLDC_Initialize(void) {
	// initialize variables
	bldc.angle = 0;
	bldc.advanceAngle = ANGLE_90DEG;

	// initialize pi controller
	// shinano commutation
	LIB_PIInit(&bldc.iPI, 18121, 13, 1, 0, 30652, 16, 512, 10, (1<<FP_AMPLITUDE), 0);

	// initialize q current pi controller
	// aneheim: step 3276, ssv 962, t 3ms, 100Hz, 8kHz
	LIB_PIInit(&bldc.qPI, 19318, 11, 0, 0, 16519, 15, 512, 10, (1<<FP_AMPLITUDE), -(1<<FP_AMPLITUDE));

	// initialize d current pi controller
	// aneheim: step 3276, ssv 962, t 3ms, 100Hz, 8kHz
	LIB_PIInit(&bldc.dPI, 19318, 11, 0, 0, 16519, 15, 512, 10, (1<<FP_AMPLITUDE), -(1<<FP_AMPLITUDE));

}

//------------------------------------------------------------------------------
// BLDC control step
//------------------------------------------------------------------------------
void BLDC_Step(void){

	int32_t vmag;
	int phaseWithMaxPWM;

	// get control variables, rotor angle and feedbacks
	bldc.setpoint = CTRL_GetSetpoint();
	bldc.amplitude = CTRL_GetAmplitude();
	bldc.direction = CTRL_GetDirection();
	bldc.angle = SPD_GetAngle();
	bldc.phCurrent.a = FB_Get(FB_IA);
	bldc.phCurrent.b = FB_Get(FB_IB);
	bldc.phCurrent.c = FB_Get(FB_IC);

	switch (CTRL_GetMode()) {

	// -------------------------------------------------------------------------
	case CTRL_BLDC_VECTOR_VOL:		// 3 phase drive - ENCODER ONLY
		// set drive angle and write to pwm
		if (bldc.direction == CTRL_DIR_CW) bldc.angle += bldc.advanceAngle;
		else bldc.angle -= bldc.advanceAngle;
		DPWM_Set(0, bldc.amplitude, bldc.angle);
		break;
	// -------------------------------------------------------------------------
	case CTRL_BLDC_COMM_VOL:		// 2 leg commutation - HALL SENSOR ONLY
		// Set bldc commutation
		BLDC_SetCommutation();
		break;
	// -------------------------------------------------------------------------
	case CTRL_BLDC_VECTOR_ICL:
		// reconstruct missing phase
		phaseWithMaxPWM = DPWM_GetPhaseWithMaxPWM();
		switch(phaseWithMaxPWM){
		case DPWM_PHASE_A:
			bldc.phCurrent.a = -bldc.phCurrent.b -bldc.phCurrent.c;
			break;
		case DPWM_PHASE_B:
			bldc.phCurrent.b = -bldc.phCurrent.a -bldc.phCurrent.c;
			break;
		case DPWM_PHASE_C:
			bldc.phCurrent.c = -bldc.phCurrent.a -bldc.phCurrent.b;
			break;
		}
		// transform from abc to dq
		LIB_ABCtoQD0(&bldc.phCurrent, bldc.angle);
		// run current regulators
		LIB_PIStep(bldc.setpoint, bldc.phCurrent.q, &bldc.qPI);
		LIB_PIStep(0, bldc.phCurrent.d, &bldc.dPI);
		// limit qd magnitude to +1/-1
		vmag  = ((int32_t)bldc.qPI.output*bldc.qPI.output);
		vmag += ((int32_t)bldc.dPI.output*bldc.dPI.output);
		vmag  = LIB_Sqrt32(vmag);
		if (vmag > (1<<FP_AMPLITUDE)){
			bldc.qPI.output = (int16_t)((((int32_t)bldc.qPI.output)<<FP_AMPLITUDE)/vmag);
			bldc.dPI.output = (int16_t)((((int32_t)bldc.dPI.output)<<FP_AMPLITUDE)/vmag);
		}
		// write to pwm module
		DPWM_Set(bldc.qPI.output, bldc.dPI.output, bldc.angle);
		break;
	// -------------------------------------------------------------------------
	case CTRL_BLDC_COMM_ICL:		// 2 leg commutation - hall sensor - current closed loop
		// check if less than threshold, drive PWMs to zero, reset PI
		if (bldc.setpoint < MIN_CURRENT_THRESHOLD){
			bldc.amplitude = 0;
			bldc.iPI.state = 0;
		} else {
			LIB_PIStep(bldc.setpoint, bldc.feedback, &bldc.iPI);
			bldc.amplitude = bldc.iPI.output;
		}
		// Set bldc commutation
		BLDC_SetCommutation();
		break;
	}

}

//------------------------------------------------------------------------------
// Get advance angle
//------------------------------------------------------------------------------
int16_t BLDC_GetAdvanceAngle(void) {
	return bldc.advanceAngle;
}

//------------------------------------------------------------------------------
// Set advance angle
//------------------------------------------------------------------------------
void BLDC_SetAdvanceAngle(int16_t angle){
	if ((angle >= 0 ) && (angle <= ANGLE_360DEG)){
		bldc.advanceAngle = angle;
	}
}

//------------------------------------------------------------------------------
// BLDC Set Commutation
//------------------------------------------------------------------------------
void BLDC_SetCommutation(void){
	int index;

	bldc.currentIndex = SPD_GetHallEdgeIndex();
	// get next drive angle based on current edge angle
	index = SPD_GetHallEdgeIndex();
	if (bldc.direction == CTRL_DIR_CW) index +=2;
	if (bldc.direction == CTRL_DIR_CCW) index -=2;
	if (index >5) index -=6;
	if (index <0) index +=6;

	// disable/enable pwm channels accordingly
	if (bldc_phase_state[index].phA_state == STATE_FLOAT) {
		PWM_DisableChannel(PWM_CH1);
	}
	if (bldc_phase_state[index].phB_state == STATE_FLOAT) {
		PWM_DisableChannel(PWM_CH2);
	}
	if (bldc_phase_state[index].phC_state == STATE_FLOAT) {
		PWM_DisableChannel(PWM_CH3);
	}

	// set pwms
	if (bldc_phase_state[index].phA_state == STATE_PWM) {
		PWM_Set(bldc.amplitude,0,0);
	}
	if (bldc_phase_state[index].phB_state == STATE_PWM) {
		PWM_Set(0,bldc.amplitude,0);
	}
	if (bldc_phase_state[index].phC_state == STATE_PWM) {
		PWM_Set(0,0,bldc.amplitude);
	}

	// get feedback
	if (bldc_phase_state[index].phA_state == STATE_LOW){
		bldc.feedback = FB_Get(FB_IA);
	}
	if (bldc_phase_state[index].phB_state == STATE_LOW){
		bldc.feedback = FB_Get(FB_IB);
	}
	if (bldc_phase_state[index].phC_state == STATE_LOW){
		bldc.feedback = FB_Get(FB_IC);
	}
	bldc.lastIndex = bldc.currentIndex;
}


