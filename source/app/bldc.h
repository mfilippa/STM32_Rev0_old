//------------------------------------------------------------------------------
// BLDC .h file - MPF 03/2011
//------------------------------------------------------------------------------

#ifndef BLDC_H
#define BLDC_H

//! \addtogroup app
//! \{
//! \addtogroup bldc
//! \{

// exported functions
extern void BLDC_Initialize(void);
extern void BLDC_Step(void);
extern void BLDC_SetAdvanceAngle(int16_t angle);
extern int16_t BLDC_GetAdvanceAngle(void);

//! \}
//! \}

#endif
