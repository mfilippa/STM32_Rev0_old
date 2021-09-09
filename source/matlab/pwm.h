// -----------------------------------------------------------------------------
// PWM h file - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef _PWM_H
#define _PWM_H

//! \addtogroup hal
//! \{
//! \addtogroup pwm
//! \{

// exported enums
enum pwm_channels {
	PWM_NONE = 0,
	PWM_CH1,
	PWM_CH2,
	PWM_CH3};

// exported prototypes
extern void PWM_Initialize(void);
extern void PWM_Set(int16_t val1, int16_t val2, int16_t val3);
extern void PWM_SetOutputState(int state);
extern void PWM_DisableChannel(int channel);
extern void PWM_SetBreakIRQEnableState(int state);

//! \}
//! \}

#endif
