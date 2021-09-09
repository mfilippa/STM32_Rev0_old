// -----------------------------------------------------------------------------
// adc.h - MPF - Nov 19, 2011
// -----------------------------------------------------------------------------

#ifndef _ADC_H_
#define _ADC_H_

//! \addtogroup hw
//! \{
//! \addtogroup adc
//! \{

// exported definitions

// exported functions
extern    void ADC_Initialize(void);
extern int16_t ADC_Read(int channel);
extern int16_t JADC1_Read(int channel);
extern int16_t JADC2_Read(int channel);
extern int16_t JADC3_Read(int channel);
extern    void ADC_EnableIRQ(void);
extern    void JADC_EnableIRQ(void);

//! \}
//! \}

#endif // _ADC_H_


