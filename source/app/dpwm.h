//------------------------------------------------------------------------------
// PWM h file - MPF 08/2011
//------------------------------------------------------------------------------

#ifndef DPWM_H
#define DPWM_H

//! \addtogroup app
//! \{
//! \addtogroup dpwm
//! \{

// public definitions
enum dwpm_enum {
	DPWM_SPWM = 0,
	DPWM_SVPWM,
	DPWM_THIPWM,
	DPWM_DPWMMIN,
	DPWM_DPWM1,
	DPWM_COUNT
};

// phases
enum dpwm_phases {
	DPWM_PHASE_A = 0,
	DPWM_PHASE_B,
	DPWM_PHASE_C,
};

// exported functions
extern void DPWM_Initialize(void);
extern void DPWM_Set(int16_t qvalue, int16_t dvalue, int16_t angle);
extern void DPWM_SetMode(int mode);
extern int16_t DPWM_Get(int phase);
extern int	DPWM_GetPhaseWithMaxPWM(void);

//! \}
//! \}

#endif
