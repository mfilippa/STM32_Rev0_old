//------------------------------------------------------------------------------
// AC Drive - MPF 08/2011
//------------------------------------------------------------------------------

// include files
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "lib.h"
#include "acdr.h"
#include "ctrl.h"
#include "acdr_tables.h"
#include "dpwm.h"
#include "comm.h"

// module structure
struct acdr_struc {
	int16_t angle;
	int32_t angle_l;
	int16_t tstep;
} acdr;

//------------------------------------------------------------------------------
// initialize module
//------------------------------------------------------------------------------
void ACDR_Initialize(void) {
	// initialize variables
	acdr.angle = acdr.angle_l = 0;
	acdr.tstep = LIB_InitStepTime(CTRL_STEP_FREQ);
}

//------------------------------------------------------------------------------
// AC control step
//------------------------------------------------------------------------------
void ACDR_Step(void){
	int16_t freq, amp;
	int mode;

	// get control variables
	freq = CTRL_GetFrequency();
	amp = CTRL_GetAmplitude();
	mode = CTRL_GetMode();

	// increment angle
	LIB_StepAngle(&acdr.angle_l, freq, acdr.tstep);
	acdr.angle = (int16_t) ((int32_t)acdr.angle_l>>16);
	// if running V/Hz, get amplitude from lookup table
	if (mode == CTRL_ACDRIVE_VHZ) {
		amp = LIB_TableLookUp(freq, acdr_vhztable, ACDR_VHZ_TABLE_SIZE);
	}
	// generate PWMs
	DPWM_Set(0, amp, acdr.angle);

	// write to debug qd0
	qd0.a = FB_Get(FB_IA);
	qd0.b = FB_Get(FB_IB);
	qd0.c = FB_Get(FB_IC);
	qd0.angle = acdr.angle;
	LIB_ABCtoQD0(&qd0, acdr.angle);
}

//------------------------------------------------------------------------------
// set angle
//------------------------------------------------------------------------------
void ACDR_SetAngle(int16_t angle){
	while (angle > ANGLE_360DEG) angle -= ANGLE_360DEG;
	while (angle < 0) angle += ANGLE_360DEG;
	acdr.angle = angle;
	acdr.angle_l = ((int32_t)angle)<< 16;
}

//------------------------------------------------------------------------------
// get angle
//------------------------------------------------------------------------------
int16_t ACDR_GetAngle(void){
	return acdr.angle;
}
