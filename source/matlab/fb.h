//------------------------------------------------------------------------------
// Feedbacks .h file - MPF 08/2011
//------------------------------------------------------------------------------

#ifndef _FB_H    // define header to include only once
#define _FB_H

//! \addtogroup hal
//! \{
//! \addtogroup fb
//! \{

// feedback channels, injected first, regular follows
enum fb_enum {
	// fast (injected) channels
	FB_IA = 0,
	FB_IB,
	FB_IC,
	// slow (regular) channels
	FB_VBUS,
	FB_HST,
	FB_VA,
	FB_VB,
	FB_VC,
	FB_COUNT
};

// public prototypes -> define as extern
extern     void FB_Initialize(void);
extern int16_t FB_Get(int index);
extern int16_t FB_GetAverage(int index);
extern int16_t FB_GetFilter(int index);
extern     void FB_InitOffsets(void);

// functions called by isr
extern void FB_ReadFastADC(void);
extern void FB_ReadSlowADC(void);

//! \}
//! \}

#endif


