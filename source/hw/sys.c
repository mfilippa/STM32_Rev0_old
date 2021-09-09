// -----------------------------------------------------------------------------
// sys.c - MPF - Nov 24, 2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "sys.h"
#include "../hw/adc.h"
#include "../hw/hwcfg.h"

// definitions

// module structure

// prototypes

// -----------------------------------------------------------------------------
// configure clocks
// -----------------------------------------------------------------------------
void SYS_Initialize(void){
	HWCFG_ConfigureClocks();
}

// -----------------------------------------------------------------------------
// enable all interrupts
// -----------------------------------------------------------------------------
void SYS_EnableIRQs(void){
	ADC_EnableIRQ();
	JADC_EnableIRQ();
	HWCFG_EnableInterrupts();
}

// -----------------------------------------------------------------------------
// disable all interrupts
// -----------------------------------------------------------------------------
void SYS_DisableIRQs(void){
	HWCFG_DisableInterrupts();
}
