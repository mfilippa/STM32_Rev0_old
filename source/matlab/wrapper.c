//------------------------------------------------------------------------------
// wrapper file - MPF 03/2011
//------------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "wrapper.h"
#include "irq.h"
#include "../app/dpwm.h"
#include "../app/main.h"
#include "../app/lib.h"
#include "../app/ctrl.h"
#include "../app/acdr.h"

#define FAST_IRQ_COUNT	(1)		// 8kHz
#define SLOW_IRQ_COUNT  (4)		// 2kHz
#define SYS_IRQ_COUNT (8)		// 1kHz

// matlab ports
double * pIn;
double * pOut;

// module structure
struct wrapper_struct {
	int32_t timestamp;
	int fastIrqCount;
	int slowIrqCount;
	int sysTimerCount;
} wrap;

// debug variables
double inp1, inp2, inp3;
double out1, out2, out3;

//------------------------------------------------------------------------------
// Wrapper initialize
//------------------------------------------------------------------------------
void WRAP_Initialize(double * in, double * out){

	// get pointers to in/out
	pIn = in;
	pOut = out;

	// initialize all
	init();
	wrap.fastIrqCount = 0;
	wrap.slowIrqCount = 0;
	wrap.sysTimerCount = 0;
	wrap.timestamp = 0;

	// -------------------------------------------------------------------------
	// initial commands
//	CTRL_SetMode(CTRL_BLDC_VECTOR_VOL);
//	CTRL_SetAmplitude((double)0.5*(1<<FP_AMPLITUDE));

	CTRL_SetMode(CTRL_BLDC_VECTOR_ICL);
	CTRL_SetSetpoint((double)0.5*(1<<FP_CURRENT));

//	CTRL_SetMode(CTRL_ACDRIVE_OL);
//	CTRL_SetAmplitude((double)0.2*(1<<FP_AMPLITUDE));
//	ACDR_SetAngle(ANGLE_90DEG);

	DPWM_SetMode(DPWM_SPWM);
	CTRL_StartStop(1);
	// -------------------------------------------------------------------------
}

//------------------------------------------------------------------------------
// Wrapper step
//------------------------------------------------------------------------------
void WRAP_Step(void){

	// get debug inputs
	inp1 = *(pIn+27);
	inp2 = *(pIn+28);
	inp3 = *(pIn+29);

	// increment simulation timestamp
	wrap.timestamp++;

	// -------------------------------------------------------------------------
	// commands -> if (wrap.timestamp == n) Function();
//	if (wrap.timestamp == (2*16000)) CTRL_SetAmplitude((double)0.1*(1<<FP_AMPLITUDE));


	// -------------------------------------------------------------------------

	// fast IRQ
	if (wrap.fastIrqCount == 0) {
		IRQ_Fast();
		wrap.fastIrqCount = FAST_IRQ_COUNT-1;
	} else {
		wrap.fastIrqCount--;
	}

	// slow IRQ
	if (wrap.slowIrqCount == 0) {
		IRQ_Slow();
		wrap.slowIrqCount = SLOW_IRQ_COUNT-1;
	} else {
		wrap.slowIrqCount--;
	}

	// sys timer IRQ
	if (wrap.sysTimerCount == 0) {
		IRQ_SystemTimer();
		wrap.sysTimerCount = SYS_IRQ_COUNT-1;
	} else {
		wrap.sysTimerCount--;
	}

	// write debug vars
	*(pOut+27) = out1;
	*(pOut+28) = out2;
	*(pOut+29) = out3;

}


