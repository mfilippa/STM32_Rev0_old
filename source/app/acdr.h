//------------------------------------------------------------------------------
// AC Drive .h file - MPF 03/2011
//------------------------------------------------------------------------------

#ifndef ACDR_H
#define ACDR_H

//! \addtogroup app
//! \{
//! \addtogroup acdr
//! \{


// exported functions
extern void ACDR_Initialize(void);
extern void ACDR_Step(void);
extern void ACDR_SetAngle(int16_t angle);
extern int16_t ACDR_GetAngle(void);

//! \}
//! \}

#endif
