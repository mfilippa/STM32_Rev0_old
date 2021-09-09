// -----------------------------------------------------------------------------
// IO functions - MPF 08/2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "io.h"
#include "../hw/gpio.h"
#include "../hw/tmr.h"

// io select - NOTE: MUST BE OREDERED TO MATCH ENUM IN .h FILE!
const struct ioSelect_struct {
	int gpio;
} ioSelect[IO_COUNT] = {
	GPIO_PB9, 	// IO_LED
	GPIO_PB8, 	// IO_DEBUG
	GPIO_PA12,	// IO_INRUSH
	GPIO_PB5,	// IO_BEMF_TRANSISTOR
	GPIO_PA11	// IO_GATEDRIVE
};

// module structure

// prototypes

// -----------------------------------------------------------------------------
// initialize IO module
// -----------------------------------------------------------------------------
void IO_Initialize(void){
	GPIO_Initialize();
}

// -----------------------------------------------------------------------------
// set io
// -----------------------------------------------------------------------------
void IO_Set(int io){
	if (io>=IO_COUNT) return;
	GPIO_Set(ioSelect[io].gpio);
}

// -----------------------------------------------------------------------------
// reset io
// -----------------------------------------------------------------------------
void IO_Reset(int io){
	if (io>=IO_COUNT) return;
	GPIO_Reset(ioSelect[io].gpio);
}

// -----------------------------------------------------------------------------
// toggle io
// -----------------------------------------------------------------------------
void IO_Toggle(int io) {
	if (io>=IO_COUNT) return;
	GPIO_Toggle(ioSelect[io].gpio);
}

// -----------------------------------------------------------------------------
// read io input
// -----------------------------------------------------------------------------
int IO_ReadInput(int io){
	if (io>=IO_COUNT) return 0;
	return GPIO_ReadInput(ioSelect[io].gpio);
}

// -----------------------------------------------------------------------------
// read io output
// -----------------------------------------------------------------------------
int IO_ReadOutput(int io){
	if (io>=IO_COUNT) return 0;
	return GPIO_ReadOutput(ioSelect[io].gpio);
}

