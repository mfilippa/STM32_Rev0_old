//------------------------------------------------------------------------------
// linear motor drive .h file - MPF 03/2011
//------------------------------------------------------------------------------

#ifndef LINEAR_H
#define LINEAR_H

//! \addtogroup app
//! \{
//! \addtogroup linear
//! \{


// exported functions
extern void LINEAR_Initialize(void);
extern void LINEAR_Step(void);
extern void LINEAR_SetPosAngle(int16_t posAngle);
extern void LINEAR_SetNegAngle(int16_t negAngle);
extern void LINEAR_SetPosEndAngle(int16_t posEndAngle);
extern void LINEAR_SetNegEndAngle(int16_t negEndAngle);

//! \}
//! \}

#endif
