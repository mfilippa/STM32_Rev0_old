// -----------------------------------------------------------------------------
// enc.h - MPF - Nov 19, 2011
// -----------------------------------------------------------------------------

#ifndef _ENC_H_
#define _ENC_H_

//! \addtogroup hw
//! \{
//! \addtogroup enc
//! \{

// exported definitions

// exported functions
extern     void ENC_Initialize(int encoderPPR);
extern uint16_t ENC_ReadCounter(void);
extern     void ENC_SetPPR(int ppr);
extern     void ENC_ResetCounter(void);

//! \}
//! \}

#endif // _ENC_H_
