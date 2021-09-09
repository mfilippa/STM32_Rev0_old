//------------------------------------------------------------------------------
// Software library .h file - MPF 08/2011
//------------------------------------------------------------------------------

#ifndef LIB_H    // define header to include only once
#define LIB_H

//! \addtogroup app
//! \{
//! \addtogroup lib
//! \{

// slew rate limiter structure
struct lib_srlstruct {
    int32_t currentValue;
    int32_t targetValue;
    int32_t stepValue;
};

// table lookup structure
struct lib_tlookupstruct {
	int16_t x;       		// x value
	int16_t y;       		// y value
	int16_t slope;   		// slope
	int16_t slope_fp;		// slope FP
};

// PI structure
struct lib_pistruct {
	int16_t k;			// controller gain
	int16_t k_fp;
	int16_t b;			// setpoint weight (0-1)
	int16_t b_fp;
	int16_t ki;			// integral constant
	int16_t ki_fp;
	int16_t kt;    		// tracking gain
	int16_t kt_fp;
	int32_t state;		// integral state
	int16_t max;		// max value
	int16_t min;		// min value
	int16_t output;		// PI output
};

// abcqd0 struct
struct lib_abcqd0struct {
	int16_t a;
	int16_t b;
	int16_t c;
	int16_t q;
	int16_t d;
	int16_t z;
	int16_t angle;
};	

// biquad struct
struct lib_biquadstruct {
	int16_t b0;
	int16_t b1;
	int16_t b2;
	int16_t a1;
	int16_t a2;
	int16_t fp;			// FP for filter coefficients
	int16_t x_z1;			// memory location x(-1)
	int16_t x_z2;			// memory location x(-2)
	int16_t y_z1;			// memory location y(-1)
	int16_t y_z2;			// memory location y(-2)
	int16_t out;
	int32_t fraction;		// for fraction saving
};		

// public prototypes
extern void LIB_Initialize(void);
extern int16_t LIB_Abs16(int16_t n);
extern     int LIB_SRLStep	(struct lib_srlstruct * srl);
extern int16_t LIB_TableLookUp(int16_t value, const struct lib_tlookupstruct * table, int tsize);
extern     int LIB_StepAngle	(int32_t * angle, int16_t frequency, int16_t step_time);
extern int16_t LIB_InitStepTime(int16_t freq);
extern int16_t LIB_SinAngle	(int16_t angle);
extern void LIB_PIInit(struct lib_pistruct * pi, int16_t k, int16_t k_fp, int16_t b,
		int16_t b_fp, int16_t ki, int16_t ki_fp, int16_t kt, int16_t kt_fp,
		int16_t max, int16_t min);
extern int16_t LIB_PIStep		(int16_t setpoint, int16_t feedback, struct lib_pistruct * pi);
extern  void LIB_ABCtoQD0	(struct lib_abcqd0struct * variable, int16_t angle);
extern  void LIB_QD0toABC	(struct lib_abcqd0struct * variable, int16_t angle);
extern int16_t LIB_BiquadFilter(struct lib_biquadstruct * filter, int16_t input);
extern int32_t LIB_Sqrt32 (int32_t n);
extern int16_t LIB_Atan2(int16_t y, int16_t x);

//! \}
//! \}

#endif

