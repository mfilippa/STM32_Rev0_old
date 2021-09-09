// -----------------------------------------------------------------------------
// Interrupts h file - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef _IRQ_H
#define _IRQ_H

//! \addtogroup hal
//! \{
//! \addtogroup irq
//! \{

// exported functions
extern void IRQ_Initialize(void);
extern void IRQ_Fast(void);
extern void IRQ_Slow(void);
extern void IRQ_PWMFault(void);
extern void IRQ_SystemTimer(void);

//! \}
//! \}

#endif
