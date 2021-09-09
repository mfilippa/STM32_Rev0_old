//------------------------------------------------------------------------------
// lib.c - MPF 08/2011
//------------------------------------------------------------------------------

// include files
#include "../stdint.h"
#include "../globals.h"
#include "lib.h"
#include "lib_tables.h"

// definitions (if public, define in header file and not here)
#define ANGLE2OFFSET           20862	// angle to table offset for sine table
#define ANGLE2OFFSET_FP           17 	// = ANGLE_360DEG into 2048
#define RATIO2OFFSET           32752	// ratio to table offset for atan table
#define RATIO2OFFSET_FP           18 	// = FP14 into 2047

//------------------------------------------------------------------------------
// initialize library
//------------------------------------------------------------------------------
void LIB_Initialize(void){
	// nothing to initialize
}

//------------------------------------------------------------------------------
// abs function
//------------------------------------------------------------------------------
int16_t LIB_Abs16(int16_t n) { return n < 0 ? -n : n; }

//------------------------------------------------------------------------------
// slew rate limiter step - returns 1 if reached target
//------------------------------------------------------------------------------
int LIB_SRLStep(struct lib_srlstruct * srl){
	int32_t newvalue;
	int reachedTarget = 0;
	
    // step must be positive
	if (srl->stepValue < 0) srl->stepValue = -srl->stepValue;

	// going up
	if (srl->currentValue < srl->targetValue){
		newvalue = srl->currentValue + srl->stepValue;
		if (newvalue >= srl->targetValue){
			srl->currentValue = srl->targetValue;
			reachedTarget = 1;
		} else {
			srl->currentValue = newvalue;
			reachedTarget = 0;
		}
	} else if (srl->currentValue > srl->targetValue){
		// going down
		newvalue = srl->currentValue - srl->stepValue;
		if (newvalue <= srl->targetValue){
			srl->currentValue = srl->targetValue;
			reachedTarget = 1;
		} else {
			srl->currentValue = newvalue;
			reachedTarget = 0;
		}
	} else {
		// already at target
		reachedTarget = 1;
	}
	return reachedTarget;
}

//------------------------------------------------------------------------------
// table lookup - returns interpolated value
//------------------------------------------------------------------------------
int16_t LIB_TableLookUp(int16_t value, const struct lib_tlookupstruct * table, int tsize){
    // Table must have x=0 element, ordered ascending (x)
	// only positive values for x. If x is negative, table is mirrored
    
    int index = 0;
    int16_t temp = 0;

	// change sign if negative
	if (value<0) value = -value;	
    
    // find lowest x in table
    while ((value >= table[index+1].x) && (index < (tsize-1)))
        index++;  						
    
    // find new value = (value-x) * Slope / 2^FP + y    
    temp = ((int32_t)(value - table[index].x) * table[index].slope)>> table[index].slope_fp;
    temp += table[index].y;
           
    return temp;
}

//------------------------------------------------------------------------------
// angle step - steps angle and returns 1 with zero cross
//------------------------------------------------------------------------------
int LIB_StepAngle(int32_t * angle, int16_t frequency, int16_t step_time) {
	// angle += 2 pi freq dt
	// math for FP11 angle, FP6 freq and FP24 step

	int32_t step;
	int zeroCross = 0;
	
	// Calculate  2pi(FP11) x freq(FP6) >> Result in (FP3)
	step = ((int32_t)ANGLE_360DEG * frequency)>>(FP11+FP6-FP3);

	// Calculate 2 pi freq (FP3) x step_time(FP24) >> Result in (FP11+FP16)
	step = ((int32_t)step * step_time)>>(FP3+FP24-FP27);
	
	// add step and wrap, flag zero cross
	*angle += step;
	if (*angle > (((int32_t)ANGLE_360DEG)<<FP16)) {
		*angle -= (((int32_t)ANGLE_360DEG)<<FP16);
		zeroCross = 1;
	}
	if (*angle < 0) {
		*angle += (((int32_t)ANGLE_360DEG)<<FP16);
		zeroCross = 1;
	}

	return zeroCross;
}

//------------------------------------------------------------------------------
// initialize step time - calculates PWM period in s (FP24)
//------------------------------------------------------------------------------
int16_t LIB_InitStepTime(int16_t freq){
	// step time (FP24) = 1/pwm_freq(FP0) * FP24
	if (freq < 0) freq = -freq;
	if (freq == 0) return 0;
	return ((int32_t)(1<<FP24)/freq);
}

//------------------------------------------------------------------------------
// sine of angle
//------------------------------------------------------------------------------
int16_t LIB_SinAngle(int16_t angle){
	while(angle > ANGLE_360DEG) angle -= ANGLE_360DEG;
	while(angle < 0) angle += ANGLE_360DEG;
	return sine_table[((int32_t)angle * ANGLE2OFFSET)>>ANGLE2OFFSET_FP];
}

//------------------------------------------------------------------------------
// initialize PI
//------------------------------------------------------------------------------
void LIB_PIInit(struct lib_pistruct * pi, int16_t k, int16_t k_fp, int16_t b,
				int16_t b_fp, int16_t ki, int16_t ki_fp, int16_t kt, int16_t kt_fp,
				int16_t max, int16_t min){
	pi->k = k; 	 pi->k_fp = k_fp;
	pi->b = b; 	 pi->b_fp = b_fp;
	pi->ki = ki; pi->ki_fp = ki_fp;
	pi->kt = kt; pi->kt_fp = kt_fp;
	pi->max = max; pi->min = min;
	pi->state = pi->output = 0;
}
//------------------------------------------------------------------------------
// PI Step - returns output of PI
//------------------------------------------------------------------------------
int16_t LIB_PIStep(int16_t setpoint, int16_t feedback, struct lib_pistruct * pi){

	int16_t error;
	int16_t proportional;
	int32_t integral;
	int32_t temp;
	int track = 0;

	// calculate proportional term = k*(b*sp-fb)
	proportional = ((int32_t)pi->b*setpoint)>>pi->b_fp;
	proportional -= feedback;
	proportional = ((int32_t)pi->k*proportional)>>pi->k_fp;
	// calculate error
	error = setpoint - feedback;
	// multiply error by ki
	if (pi->ki_fp >= 16) integral = ((int32_t)error*pi->ki)>>(pi->ki_fp-FP16);
	else integral = ((int32_t)error*pi->ki)<<(FP16-pi->ki_fp);
	// calculate temporary output
	temp = ((int32_t)proportional<<FP16) + integral + pi->state;
	// saturate output
	if (temp > ((int32_t)pi->max<<FP16)) {
		pi->output = pi->max; 
		track = 1;
	} else if (temp < ((int32_t)pi->min<<FP16)) {
		pi->output = pi->min;
		track = 1;
	} else pi->output = (int16_t)(temp>>FP16);
	// calculate diference and apply tracking gain
	if (track){
		temp = ((int32_t)pi->output) - (temp>>FP16);
		if (pi->kt_fp >=16) temp = (temp*pi->kt)>>(pi->kt_fp-FP16);
		else temp = (temp*pi->kt)<<(FP16-pi->kt_fp);
	} else temp = 0;
	// calculate new state
	pi->state += integral + temp;
	return pi->output;

}

//------------------------------------------------------------------------------
// abc to qd0 transform
//------------------------------------------------------------------------------
void LIB_ABCtoQD0(struct lib_abcqd0struct * variable, int16_t angle){

	// *** q component ****************************************//
	// cos (angle) = sin (angle + 90)
	variable->q  = ((int32_t) LIB_SinAngle(angle + ANGLE_90DEG) * variable->a)>>FP14;
	// cos (angle - 120) = sin (angle - 30)
	variable->q += ((int32_t) LIB_SinAngle(angle - ANGLE_30DEG) * variable->b)>>FP14;
	// cos (angle + 120) = -sin(angle + 30)
    variable->q += ((int32_t) -LIB_SinAngle(angle + ANGLE_30DEG) * variable->c)>>FP14;
	// x 2/3
	variable->q  = ((int32_t) 21845 * variable->q)>>FP15;

	// *** d component ****************************************//
	// sin (angle)
	variable->d  = ((int32_t) LIB_SinAngle(angle) * variable->a)>>FP14;
	// sin (angle - 120)  
	variable->d += ((int32_t) LIB_SinAngle(angle - ANGLE_120DEG) * variable->b)>>FP14;
	// sin (angle + 120)	  
    variable->d += ((int32_t) LIB_SinAngle(angle + ANGLE_120DEG) * variable->c)>>FP14;
	// x 2/3	  
	variable->d  = ((int32_t) 21845 * variable->d)>>FP15;

	// *** 0 component ****************************************//
	variable->z  = variable->a >> 1;	  				// x 1/2
	variable->z += variable->b >> 1;	  				// x 1/2
    variable->z += variable->c >> 1;	  				// x 1/2
	variable->z  = ((int32_t) 21845 * variable->z)>> FP15;	// x 2/3

}

//------------------------------------------------------------------------------
// qd0 to abc transform
//------------------------------------------------------------------------------
void LIB_QD0toABC(struct lib_abcqd0struct * variable, int16_t angle){

	// *** a component ****************************************//
	// cos (angle) = sin (angle + 90)
	variable->a  = ((int32_t) LIB_SinAngle(angle + ANGLE_90DEG) * variable->q)>>FP14;
	// sin (angle) 
	variable->a += ((int32_t) LIB_SinAngle(angle) * variable->d)>>FP14;
	// 1
	variable->a += variable->z;
    
	// *** b component ****************************************//
	// cos (angle - 120) = sin (angle - 30)
	variable->b  = ((int32_t) LIB_SinAngle(angle - ANGLE_30DEG) * variable->q)>>FP14;
	// sin (angle - 120) 
	variable->b += ((int32_t) LIB_SinAngle(angle - ANGLE_120DEG) * variable->d)>>FP14;
	// 1
	variable->b += variable->z;

	// *** c component ****************************************//
	// cos (angle + 120) = - sin (angle + 30)
	variable->c  = ((int32_t) -LIB_SinAngle(angle + ANGLE_30DEG) * variable->q)>>FP14;
	// sin (angle + 120) 
	variable->c += ((int32_t) LIB_SinAngle(angle + ANGLE_120DEG) * variable->d)>>FP14;
	// 1
	variable->c += variable->z;

}

//------------------------------------------------------------------------------
// Biquad digital filter - returns result of filter
//------------------------------------------------------------------------------
int16_t LIB_BiquadFilter(struct lib_biquadstruct * filter, int16_t input){
    // Implements through Direct Form 1:
    //  H(z) = __(b0 + b1 z^-1 + b2 z^-2)__
    //           ( 1 + a1 z^-1 + a2 z^-2)
	int32_t temp;

    // y(n) = b0 x(n) + b1 x(-1) + b2 x(-2) -a1 y(-1) - a2 y(-2)
    temp  = (int32_t)filter->b0 * input;
    temp += (int32_t)filter->b1 * filter->x_z1;
    temp += (int32_t)filter->b2 * filter->x_z2;
    temp -= (int32_t)filter->a1 * filter->y_z1;
    temp -= (int32_t)filter->a2 * filter->y_z2;
	temp += filter->fraction;	// fraction saving
	filter->out = temp >> filter->fp;
	
    // shift memories
    filter->x_z2 = filter->x_z1;
    filter->x_z1 = input;
    filter->y_z2 = filter->y_z1;
    filter->y_z1 = filter->out;
	filter->fraction = temp - ((int32_t)filter->out<<filter->fp);

    // output
    return filter->out;    
}

//------------------------------------------------------------------------------
// square root - from Wilco Dijkstra (ARM), 3.2us in STM32103 @ 72MHz
//------------------------------------------------------------------------------
int32_t LIB_Sqrt32 (int32_t n) {
	#define iter1(N) try = root + (1 << (N)); \
	    if (n >= try << (N)) { n -= try << (N); root |= 2 << (N); }
	uint32_t root = 0, try;
	if (n<0) n=0;
    iter1 (15);    iter1 (14);    iter1 (13);    iter1 (12);
    iter1 (11);    iter1 (10);    iter1 ( 9);    iter1 ( 8);
    iter1 ( 7);    iter1 ( 6);    iter1 ( 5);    iter1 ( 4);
    iter1 ( 3);    iter1 ( 2);    iter1 ( 1);    iter1 ( 0);
    return root >> 1;
}

//------------------------------------------------------------------------------
// atan2 - based on Jim Shima self normalizing atan, 1.9us in STM32103 @ 72MHz
//------------------------------------------------------------------------------
int16_t LIB_Atan2(int16_t y, int16_t x){
	int16_t ratio, angle;
	if ((x==0)&&(y==0)) return 0; // undefined, return 0 degrees
	if (y>=0){
		if (x>=0){	// I quadrant
			// compute ratio = (y-x)/(x+y)
			ratio = (((int32_t)(y-x))<<FP14)/((int16_t)(x+y));
			ratio = ((1<<FP_AMPLITUDE)/2) + (ratio/2);
			ratio = ((int32_t)ratio * RATIO2OFFSET)>>RATIO2OFFSET_FP;
			// return angle
			angle = atan_table[ratio];
		} else {	// II quadrant
			// compute ratio = (y+x)/(-x+y)
			ratio = (((int32_t)(y+x))<<FP14)/((int16_t)(-x+y));
			ratio = ((1<<FP_AMPLITUDE)/2) + (ratio/2);
			ratio = ((int32_t)ratio * RATIO2OFFSET)>>RATIO2OFFSET_FP;
			// return angle
			angle = ANGLE_180DEG - atan_table[ratio];
		}
	} else {	// y < 0
		if (x>=0) {	// IV quadrant
			// compute ratio = (-y-x)/(x-y)
			ratio = (((int32_t)(-y-x))<<FP14)/((int16_t)(x-y));
			ratio = ((1<<FP_AMPLITUDE)/2) + (ratio/2);
			ratio = ((int32_t)ratio * RATIO2OFFSET)>>RATIO2OFFSET_FP;
			// return angle
			angle = ANGLE_360DEG - atan_table[ratio];
		} else {	// III quadrant
			// compute ratio = (-y+x)/(-x-y)
			ratio = (((int32_t)(-y+x))<<FP14)/((int16_t)(-x-y));
			ratio = ((1<<FP_AMPLITUDE)/2) + (ratio/2);
			ratio = ((int32_t)ratio * RATIO2OFFSET)>>RATIO2OFFSET_FP;
			// return angle
			angle = ANGLE_180DEG + atan_table[ratio];
		}
	}
	return angle;
}

