// -----------------------------------------------------------------------------
// Communications h file - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef _COMM_H
#define _COMM_H

//! \addtogroup hal
//! \{
//! \addtogroup comm
//! \{

// exported functions
extern void COMM_Initialize(void);
extern void COMM_Execute(void);
extern void COMM_SaveToBuffer(int number, int16_t value);
extern void COMM_EnableBufferWrite(void);
extern void COMM_DisableBufferWrite(void);

//! \}
//! \}

#endif

