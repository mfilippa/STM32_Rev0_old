// -----------------------------------------------------------------------------
// spd.c - MPF - Dec 4, 2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "wrapper.h"
#include "spd.h"

// definitions

// module structure
struct spd_struct {
	int16_t angle;			// tracking angle
	int16_t lastAngle;
	int revCount;		// counts revolutions at a zero cross transition
} spd;

// prototypes

// -----------------------------------------------------------------------------
// Track angle - returns 1 with CW zero cross, -1 with CCW zero cross
// -----------------------------------------------------------------------------
int SPD_TrackAngle(void){
	int zerocross;
	int32_t tempAngle;

	// get angle from matlab - assumes electrical angle input
	tempAngle = *(pIn+10)*(1<<FP_ANGLE);
	while (tempAngle >= ANGLE_360DEG) tempAngle -= ANGLE_360DEG;
	while (tempAngle < 0) tempAngle += ANGLE_360DEG;
	spd.angle = (int16_t) tempAngle;

	// check for zero cross both directions
	if (((spd.angle-ANGLE_180DEG)>=0)&&((spd.lastAngle-ANGLE_180DEG)<0)) zerocross = -1;
	if (((spd.angle-ANGLE_180DEG)<=0)&&((spd.lastAngle-ANGLE_180DEG)>0)) zerocross = 1;

	// rev counting
	spd.revCount += zerocross;

	// save last angle
	spd.lastAngle = spd.angle;

	return zerocross;
}
// -----------------------------------------------------------------------------
// Get angle
// -----------------------------------------------------------------------------
int16_t SPD_GetAngle(void){ return spd.angle;}
// -----------------------------------------------------------------------------
// Get Speed
// -----------------------------------------------------------------------------
int16_t SPD_GetSpeed(void){ return *(pIn+11)*(1<<FP_FREQUENCY); }
// -----------------------------------------------------------------------------
// set revolution count
// -----------------------------------------------------------------------------
void SPD_SetRevCount(int value){ spd.revCount = value; }
// -----------------------------------------------------------------------------
// get revolution count
// -----------------------------------------------------------------------------
int SPD_GetRevCount(void){ return spd.revCount;}

// not used
void SPD_Initialize(void){ }
int SPD_GetSensor(void){ return 0;}
void SPD_ZeroAngle(void){ }
void SPD_SetSensor(int sensor){ }

// hall sensor specific
int SPD_GetHallCode(void){ return 0;}
int SPD_GetHallCodeAngle(int index){ return 0;}
void SPD_StoreHallAngleData(int hallAngleIndex){ }
void SPD_BuildHallCodeTable(void){ }
int SPD_GetHallEdgeIndex(void){ return 0;}

// encoder specific
void SPD_SetEncoderToAngleScaling(int16_t value, int16_t fp){ }
void SPD_EncoderChangeSign(void){ }
