// -----------------------------------------------------------------------------
// Fault handler - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef FAULT_H
#define FAULT_H

//! \addtogroup app
//! \{
//! \addtogroup fault
//! \{

// exported faults
#define FAULT_NONE		   ((uint16_t)0x0000)	// no faults
#define FAULT_GATE_DRIVE   ((uint16_t)0x0001)  	// Gate driver fault
#define FAULT_OVERCURRENT  ((uint16_t)0x0002)  	// IGBT overcurrent fault
#define FAULT_OVERVOLTAGE  ((uint16_t)0x0004)  	// Bus overvoltage fault
#define FAULT_AMBOVERTEMP  ((uint16_t)0x0008)  	// Ambient temp out of range
#define FAULT_HSOVERTEMP   ((uint16_t)0x0010)  	// Heatsink temp out of range
#define FAULT_ALL_FAULTS   ((uint16_t)0xFFFF)  	// Sets/clears all faults

// exported functions
extern void FAULT_Initialize(void);
extern void FAULT_Set(uint16_t FaultCode);
extern uint16_t FAULT_Get(void);
extern void FAULT_Clear(uint16_t FaultCode);
extern void FAULT_FastMonitor(void);
extern void FAULT_SlowMonitor(void);

//! \}
//! \}

#endif 

