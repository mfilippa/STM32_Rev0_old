//------------------------------------------------------------------------------
// wrapper h file - MPF 03/2011
//------------------------------------------------------------------------------
//! @addtogroup matlab
//! @{
//! @addtogroup wrapper
//! @{

#ifndef WRAPPER_H
#define WRAPPER_H

// exported matlab variables
extern double * pIn;
extern double * pOut;

// debug variables
extern double inp1, inp2, inp3;
extern double out1, out2, out3;

// exported functions
extern void WRAP_Initialize(double * in, double * out);
extern void WRAP_Step(void);

#endif

//! @}
//! @}
