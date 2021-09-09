//------------------------------------------------------------------------------
// Linear motor drive - MPF 08/2011
//------------------------------------------------------------------------------

// include files
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "lib.h"
#include "linear.h"
#include "ctrl.h"
#include "dpwm.h"
#include "comm.h"
#include "acdr.h"

// defines
#define LINEAR_TIMEOUT	     40000	// 5sec at 8kHz
#define LINEAR_RUN_TIMEOUT	  8000  // 1sec at 8kHz

// state machine states
enum linear_states {
	LINEAR_STATE_FINDPOS,	// run 1Hz till zero or timeout of 5sec
	LINEAR_STATE_FINDNEG,	// run -1Hz till zero angle or timeout of 5sec
	LINEAR_STATE_POS,		// go positive frequency
	LINEAR_STATE_NEG,		// go negative frequency
};

// module structure
struct linear_struc {
	int16_t angle;
	int16_t lastAngle;
	int16_t angleOffset;
	int16_t state;
	int32_t timeout;
	int16_t posAngle;
	int16_t negAngle;
	int16_t posEndAngle;
	int16_t negEndAngle;
} linear;

//------------------------------------------------------------------------------
// initialize module
//------------------------------------------------------------------------------
void LINEAR_Initialize(void) {
	// initialize variables
	linear.state = LINEAR_STATE_FINDPOS;
	linear.timeout = LINEAR_TIMEOUT;
	linear.posAngle = 8000;
	linear.negAngle = 0;
	linear.posEndAngle = ANGLE_330DEG;
	linear.negEndAngle = ANGLE_30DEG;
	CTRL_SetFrequency(64);
}

//------------------------------------------------------------------------------
// linear control step
//------------------------------------------------------------------------------
void LINEAR_Step(void){
	int midstroke = 0;
	int zerocross = 0;

	// get external vars
	linear.angle = SPD_GetAngle();
	if ((linear.angle<ANGLE_30DEG)&&(linear.lastAngle>ANGLE_330DEG)) zerocross = 1;
	else if ((linear.angle>ANGLE_330DEG)&&(linear.lastAngle<ANGLE_30DEG)) zerocross = -1;
	else if ((linear.angle>=ANGLE_180DEG)&&(linear.lastAngle<ANGLE_180DEG)) midstroke = 1;
	else if ((linear.angle<=ANGLE_180DEG)&&(linear.lastAngle>ANGLE_180DEG)) midstroke = -1;

	// state machine
	switch (linear.state){
		// find positive end ---------------------------------------------------
	case LINEAR_STATE_FINDPOS:
		if ((linear.timeout--) == 0) {
			linear.timeout = LINEAR_TIMEOUT;
			CTRL_SetFrequency(-64);
			linear.state = LINEAR_STATE_FINDNEG;
		}
		if (midstroke!=0){
			CTRL_SetFrequency(0);
			linear.timeout = LINEAR_RUN_TIMEOUT;
			linear.state = LINEAR_STATE_POS;
		}
		ACDR_Step();
		break;
		// find negative end ---------------------------------------------------
	case LINEAR_STATE_FINDNEG:
		if ((linear.timeout--) == 0) {
			linear.timeout = LINEAR_TIMEOUT;
			CTRL_SetFrequency(64);
			linear.state = LINEAR_STATE_FINDPOS;
		}
		if (midstroke!=0){
			CTRL_SetFrequency(0);
			linear.timeout = LINEAR_RUN_TIMEOUT;
			linear.state = LINEAR_STATE_POS;
		}
		ACDR_Step();
		break;
		// positive motion -----------------------------------------------------
	case LINEAR_STATE_POS:
		ACDR_SetAngle(linear.angle+linear.posAngle);
		ACDR_Step();
		if ((!zerocross)&&(linear.angle >= linear.posEndAngle)&&(linear.lastAngle < linear.posEndAngle)){
			linear.timeout = LINEAR_RUN_TIMEOUT;
			linear.state = LINEAR_STATE_NEG;
		}
		if ((linear.timeout--)==0) {
			CTRL_StartStop(0);
		}
		IO_Set(IO_DEBUG);
		break;
		// negative motion -----------------------------------------------------
	case LINEAR_STATE_NEG:
		ACDR_SetAngle(linear.angle-linear.negAngle);
		ACDR_Step();
		if ((!zerocross)&&(linear.angle <= linear.negEndAngle)&&(linear.lastAngle > linear.negEndAngle)){
			linear.timeout = LINEAR_RUN_TIMEOUT;
			linear.state = LINEAR_STATE_POS;
		}
		if ((linear.timeout--)==0) {
			CTRL_StartStop(0);
		}
		IO_Reset(IO_DEBUG);
		break;
	}

	// save last angle
	linear.lastAngle = linear.angle;

	// save to buffers
	COMM_SaveToBuffer(1,linear.angle);
	COMM_SaveToBuffer(2,FB_Get(FB_IA));
	COMM_SaveToBuffer(3,FB_Get(FB_IB));
	COMM_SaveToBuffer(4,FB_Get(FB_IC));
	COMM_SaveToBuffer(5,0);
	COMM_SaveToBuffer(6,0);
}

//------------------------------------------------------------------------------
// set pos angle
//------------------------------------------------------------------------------
void LINEAR_SetPosAngle(int16_t posAngle){
	if ((posAngle<ANGLE_360DEG)&&(posAngle>=0)) linear.posAngle = posAngle;
}

//------------------------------------------------------------------------------
// set neg angle
//------------------------------------------------------------------------------
void LINEAR_SetNegAngle(int16_t negAngle){
	if ((negAngle<ANGLE_360DEG)&&(negAngle>=0)) linear.negAngle = negAngle;
}

//------------------------------------------------------------------------------
// set pos end angle
//------------------------------------------------------------------------------
void LINEAR_SetPosEndAngle(int16_t posEndAngle){
	if ((posEndAngle<ANGLE_360DEG)&&(posEndAngle>=0)) linear.posEndAngle = posEndAngle;
}

//------------------------------------------------------------------------------
// set neg end angle
//------------------------------------------------------------------------------
void LINEAR_SetNegEndAngle(int16_t negEndAngle){
	if ((negEndAngle<ANGLE_360DEG)&&(negEndAngle>=0)) linear.negEndAngle = negEndAngle;
}






