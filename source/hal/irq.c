// -----------------------------------------------------------------------------
// Interrupts - MPF 08/2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "irq.h"
#include "fb.h"
#include "io.h"
#include "../app/sch.h"
#include "../app/fault.h"
#include "../app/ctrl.h"
#include "../hw/adc.h"

// -----------------------------------------------------------------------------
// irq initialize
// -----------------------------------------------------------------------------
void IRQ_Initialize(void){
	// nothing to initialize
}

// -----------------------------------------------------------------------------
// Fast IRQ
// -----------------------------------------------------------------------------
void IRQ_Fast(void){
	FB_ReadFastADC();
	FAULT_FastMonitor();
	CTRL_FastStep();
}

// -----------------------------------------------------------------------------
// Slow IRQ
// -----------------------------------------------------------------------------
void IRQ_Slow(void){
	FB_ReadSlowADC();
	FAULT_SlowMonitor();
	CTRL_SlowStep();
}

// -----------------------------------------------------------------------------
// Break IRQ
// -----------------------------------------------------------------------------
void IRQ_PWMFault(void){

	// BRK IRQ is LEVEL based (not edge), cannot clear flag unless BRK pin is
	// inactive (fault cleared). Also need to disable BRK IRQ to avoid entering
	// this IRQ function forever. Do not forget of Clear Pending bit before
	// re-enabling.

	CTRL_SetGateDriveEnableState(0);
	FAULT_Set(FAULT_GATE_DRIVE);
}

// -----------------------------------------------------------------------------
// system timer
// -----------------------------------------------------------------------------
void IRQ_SystemTimer(void){
	SCH_Step();
}

