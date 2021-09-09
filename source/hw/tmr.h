// -----------------------------------------------------------------------------
// Timers - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef TMR_H
#define TMR_H

//! \addtogroup hw
//! \{
//! \addtogroup tmr
//! \{

// exported defines
#define TMR1_PERIOD       2250   // 16kHz on 72Mhz clock, center aligned
#define TMR2_PERIOD       3600   // 20kHz on 72Mhz clock, edge aligned
#define TMR4_PERIOD		     4   //  4kHz

// exported enums
enum tmr1_channels {
	TMR1_NONE = 0,
	TMR1_CH1,
	TMR1_CH2,
	TMR1_CH3};

// exported states
enum tmr_states {
	TMR_DISABLE = 0,
	TMR_ENABLE
};

// exported functions
extern void TMR1_Init(void);
extern void TMR1_Set(uint16_t val1, uint16_t val2, uint16_t val3); 
extern void TMR1_SetOutputState(int state);
extern void TMR1_IRQUpdateEnable(void);
extern void TMR1_IRQCC4Enable(void);
extern void TMR1_SetBRKIRQState(int enable);
extern void TMR1_DisableChannel(int state);
extern void TMR2_Init(void);
extern void TMR2_SetCh1(uint16_t value);
extern void TMR2_SetCh2(uint16_t value);
extern void TMR4_Init(void);
extern void TMR4_IRQCC1Enable(void);
extern void TMR4_IRQCC4Enable(void);

//! \}
//! \}

#endif 



