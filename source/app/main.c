// -----------------------------------------------------------------------------
// Main	- MPF 08/2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "main.h"
#include "sch.h"
#include "fault.h"
#include "ctrl.h"
#include "acdr.h"
#include "bldc.h"
#include "dpwm.h"
#include "lib.h"
#include "comm.h"
#include "linear.h"

// -----------------------------------------------------------------------------
// init
// -----------------------------------------------------------------------------
void init(void){

	// initialize system
	SYS_Initialize();

	// initialize hal modules
	COMM_Initialize();
	FB_Initialize();
	IO_Initialize();
	IRQ_Initialize();
	PWM_Initialize();
	SPD_Initialize();

	// initialize app modules
	ACDR_Initialize();
	BLDC_Initialize();
	CTRL_Initialize();
	LINEAR_Initialize();
	DPWM_Initialize();
	FAULT_Initialize();
	LIB_Initialize();
	SCH_Initialize();

	// enable all interrupts
	SYS_EnableIRQs();

#ifndef MATLAB_SIMULATION
	// wait 250ms to initialize feedback offsets
	SCH_SetTimer(1,250);
	while (SCH_TimerHasExpired(1)==0);
	FB_InitOffsets();
#endif

}

// -----------------------------------------------------------------------------
// main
// -----------------------------------------------------------------------------
int main(void){

	// initialize all
	init();
	
#ifndef MATLAB_SIMULATION
	// execute communications in background - should never return
	COMM_Execute();
#endif
	
	return 0;
}

