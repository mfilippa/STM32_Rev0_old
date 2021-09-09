// -----------------------------------------------------------------------------
// GPIO h file - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef GPIO_H
#define GPIO_H

//! \addtogroup hw
//! \{
//! \addtogroup gpio
//! \{

// exported enums
enum gpio_pin {
	GPIO_PA11 = 0,	// gate driver enable
	GPIO_PA12,		// inrush relay
	GPIO_PB5,		// BEMF range
	GPIO_PB8,		// debug pin
	GPIO_PB9,		// LED
	GPIO_PC6,		// hall sensor
	GPIO_PC7,		// hall sensor
	GPIO_PC8,		// hall sensor
	GPIO_COUNT
};

// Exported functions 
extern void GPIO_Initialize(void);
extern void GPIO_Set(int io);
extern void GPIO_Reset(int io);
extern void GPIO_Toggle(int io);
extern int GPIO_ReadOutput(int io);
extern int GPIO_ReadInput(int io);

//! \}
//! \}

#endif // GPIO_H 

