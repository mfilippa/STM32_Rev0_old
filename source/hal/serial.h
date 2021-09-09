// -----------------------------------------------------------------------------
// serial.h - MPF - Dec 4, 2011
// -----------------------------------------------------------------------------

#ifndef _SERIAL_H_
#define _SERIAL_H_

//! \addtogroup hal
//! \{
//! \addtogroup serial
//! \{

// exported definitions

// exported functions
extern void SERIAL_Initialize(void);
extern void SERIAL_WriteByte(uint16_t data);
extern void SERIAL_WriteWord(uint16_t data);
extern uint16_t SERIAL_ReadByte(void);
extern uint16_t SERIAL_ReadWord(void);

//! \}
//! \}

#endif // _SERIAL_H_
