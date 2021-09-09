// -----------------------------------------------------------------------------
// Fault handler - MPF 08/2011
// -----------------------------------------------------------------------------

// includes 
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "fault.h"
#include "ctrl.h"

// defines
#define MAX_CURRENT		   ((int16_t)20480)	// 5A
#define MAX_VOLTAGE		   ((int16_t)24960)	// 390V
#define MAX_AMBTEMP		   ((int16_t) 8320)	// 65C
#define MAX_HSTEMP		   ((int16_t) 8320)	// 65C
#define MIN_TEMP		   ((int16_t)-5760)	// -45C

// module structure 
struct fault_struct {
	uint16_t code;
} fault;

// -----------------------------------------------------------------------------
// initialize module
// -----------------------------------------------------------------------------
void FAULT_Initialize(void){
	fault.code = FAULT_NONE;
}

// -----------------------------------------------------------------------------
// set fault
// -----------------------------------------------------------------------------
void FAULT_Set(uint16_t code){
	fault.code |= code;
}

// -----------------------------------------------------------------------------
// clear fault
// -----------------------------------------------------------------------------
void FAULT_Clear(uint16_t code){
	fault.code &= ~code;
}

// -----------------------------------------------------------------------------
// get fault
// -----------------------------------------------------------------------------
uint16_t FAULT_Get(void){
	return fault.code;
}

// -----------------------------------------------------------------------------
// monitor fast faults
// -----------------------------------------------------------------------------
void FAULT_FastMonitor(void){
	int16_t value;

	// overcurrent fault
	value = FB_Get(FB_IA);
	if ((value > MAX_CURRENT) || (value < (-MAX_CURRENT))){
		FAULT_Set(FAULT_OVERCURRENT);
		CTRL_SetGateDriveEnableState(0);
	}

	value = FB_Get(FB_IB);
	if ((value > MAX_CURRENT) || (value < (-MAX_CURRENT))){
		FAULT_Set(FAULT_OVERCURRENT);
		CTRL_SetGateDriveEnableState(0);
	}

	value = FB_Get(FB_IC);
	if ((value > MAX_CURRENT) || (value < (-MAX_CURRENT))){
		FAULT_Set(FAULT_OVERCURRENT);
		CTRL_SetGateDriveEnableState(0);
	}
}

// -----------------------------------------------------------------------------
// monitor slow faults
// -----------------------------------------------------------------------------
void FAULT_SlowMonitor(void){
	int16_t temp;
	
	// overvoltage fault
	temp = FB_Get(FB_VBUS);
	if (temp > MAX_VOLTAGE) {
		FAULT_Set(FAULT_OVERVOLTAGE);
		CTRL_SetGateDriveEnableState(0);
	}
	
	// heatsink overtemp fault
	temp = FB_Get(FB_HST);
	if ((temp > MAX_HSTEMP) || (temp < MIN_TEMP)) {
		FAULT_Set(FAULT_HSOVERTEMP);
		CTRL_SetGateDriveEnableState(0);
	}
}


