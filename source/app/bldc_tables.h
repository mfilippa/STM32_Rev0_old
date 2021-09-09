// -----------------------------------------------------------------------------
// BLDC tables h file - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef BLDC_TABLES_H
#define BLDC_TABLES_H

// definitions
#define STATE_PWM		 1
#define STATE_LOW		 0
#define STATE_FLOAT		-1

// bldc phase state table structure
struct bldc_phase_state_struct {
	int phA_state;
	int phB_state;
	int phC_state;
};

// bldc commutation drive angles
const int16_t bldc_angles[6] = {ANGLE_0DEG, ANGLE_60DEG, ANGLE_120DEG, ANGLE_180DEG,
				       	      ANGLE_240DEG, ANGLE_300DEG};

// commutation table, corresponding to bldc angles
const struct bldc_phase_state_struct bldc_phase_state[6] = {
		{STATE_FLOAT , STATE_LOW , STATE_PWM},
		{STATE_PWM , STATE_LOW , STATE_FLOAT} ,
		{STATE_PWM , STATE_FLOAT , STATE_LOW} ,
		{STATE_FLOAT , STATE_PWM , STATE_LOW} ,
		{STATE_LOW , STATE_PWM , STATE_FLOAT} ,
		{STATE_LOW , STATE_FLOAT , STATE_PWM},
};


#endif
