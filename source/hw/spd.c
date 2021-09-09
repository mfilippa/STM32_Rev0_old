// -----------------------------------------------------------------------------
// Speed / angle feedbacks - MPF 08/2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "spd.h"
#include "io.h"
#include "fb.h"
#include "../app/lib.h"
#include "../hw/enc.h"
#include "../hw/gpio.h"

// anaheim servo motor
#define ENCODER_PPR			  2500
#define ENC_COUNT_TO_ANGLE   21081		// enc count to electrical angle
#define ENC_COUNT_TO_ANGLE_FP   11		// ... includes # poles

// shinano motor
//#define ENCODER_PPR			   400
//#define ENC_COUNT_TO_ANGLE   16469		// enc count to electrical angle
//#define ENC_COUNT_TO_ANGLE_FP    9		// ... includes # poles


// module structure
struct spd_struct {
	     int fbSensor;		// feedback sensor
	int16_t angle;			// tracking angle
	int16_t lastAngle;
		 int revCount;		// counts revolutions at a zero cross transition

	// encoder specific
	int16_t encToAngle;		// include encoder PPR and motor # poles
	int16_t encToAngleFp;

	// hall angle specific
	int lastHallCode;
	int currentHallCode;
	int edgeIndex;
	int hallCodeSequence[6];
	struct hallCodeTable_struct {
		int codeAngleIndex;
		int lastCCWCode;
		int lastCWCode;
		int CCWEdgeIndex;
		int CWEdgeIndex;
	} hallCodeTable[6];
} spd;

const int16_t hallAngle[6] = { ANGLE_30DEG, ANGLE_90DEG, ANGLE_150DEG,
								ANGLE_210DEG, ANGLE_270DEG, ANGLE_330DEG };
const int16_t edgeAngle[6] = { ANGLE_0DEG, ANGLE_60DEG, ANGLE_120DEG,
								ANGLE_180DEG, ANGLE_240DEG, ANGLE_300DEG };

// prototypes
int SPD_TrackSinCosAngle(void);

// -----------------------------------------------------------------------------
// Initialize speed module
// -----------------------------------------------------------------------------
void SPD_Initialize(void){

	// initializations
	ENC_Initialize(ENCODER_PPR);
	SPD_SetEncoderToAngleScaling(ENC_COUNT_TO_ANGLE,ENC_COUNT_TO_ANGLE_FP);
	SPD_SetSensor(SPD_HALLFB);

	// initialize variables
	spd.currentHallCode = spd.lastHallCode = 0;
	spd.angle = spd.lastAngle = spd.edgeIndex = 0;
	spd.revCount = 0;
}

// -----------------------------------------------------------------------------
// Get hall sensor code - return code
// -----------------------------------------------------------------------------
int SPD_GetHallCode(void){
	int code;
	// get state, range check, force unknown states to -1
	code = GPIO_ReadInput(GPIO_PC6) + (GPIO_ReadInput(GPIO_PC7)<<1) + (GPIO_ReadInput(GPIO_PC8)<<2) - 1;
	if ((code < 0) || (code >5)) code = -1;
	return code;
}

// -----------------------------------------------------------------------------
// Get angle
// -----------------------------------------------------------------------------
int16_t SPD_GetAngle(void){ return spd.angle; }

// -----------------------------------------------------------------------------
// Get Speed
// -----------------------------------------------------------------------------
int16_t SPD_GetSpeed(void){ return 0; }

// -----------------------------------------------------------------------------
// Track hall angle - returns 1 with CW zero cross, -1 with CCW zero cross
// -----------------------------------------------------------------------------
int SPD_TrackHallAngle(void){

	int zerocross = 0;

	// update angle if a new state is detected
	spd.currentHallCode = SPD_GetHallCode();
	if (spd.currentHallCode != spd.lastHallCode) {
		// determine direction from last hall code
		if (spd.lastHallCode == spd.hallCodeTable[spd.currentHallCode].lastCWCode){
			// CW direction
			spd.edgeIndex = spd.hallCodeTable[spd.currentHallCode].CWEdgeIndex;
			spd.angle = edgeAngle[spd.edgeIndex];
		} else if (spd.lastHallCode == spd.hallCodeTable[spd.currentHallCode].lastCCWCode){
			// CCW direction
			spd.edgeIndex = spd.hallCodeTable[spd.currentHallCode].CCWEdgeIndex;
			spd.angle = edgeAngle[spd.edgeIndex];
		} else {
			// no valid transition detected, default to hallCodeAngle
			spd.edgeIndex = spd.hallCodeTable[spd.currentHallCode].codeAngleIndex;
			spd.angle = hallAngle[spd.edgeIndex];
		}
		// check for zero cross, both directions
		if ((spd.angle>=0)&&(spd.lastAngle<0)) zerocross = -1;
		if ((spd.angle<=0)&&(spd.lastAngle>0)) zerocross = 1;
		// save last values
		spd.lastHallCode = spd.currentHallCode;
		spd.lastAngle = spd.angle;
	}
	return zerocross;
}

// -----------------------------------------------------------------------------
// get hall edge angle index
// -----------------------------------------------------------------------------
int SPD_GetHallEdgeIndex(void){
	return spd.edgeIndex;
}

// -----------------------------------------------------------------------------
// Track encoder angle - returns 1 with CW zero cross, -1 with CCW zero cross
// -----------------------------------------------------------------------------
int SPD_TrackEncoderAngle(void){

	int32_t angle;
	int zerocross = 0;

	// get angle from encoder reading
	angle = ((int32_t) ENC_ReadCounter() * spd.encToAngle )>>spd.encToAngleFp;
	while (angle > ANGLE_360DEG) angle -= ANGLE_360DEG;
	while (angle < 0 ) angle += ANGLE_360DEG;
	spd.angle = (int16_t) angle;

	// check for zero cross both directions
	if (((spd.angle-ANGLE_180DEG)>=0)&&((spd.lastAngle-ANGLE_180DEG)<0)) zerocross = -1;
	if (((spd.angle-ANGLE_180DEG)<=0)&&((spd.lastAngle-ANGLE_180DEG)>0)) zerocross = 1;

	// save angle
	spd.lastAngle = spd.angle;

	// return zerocross
	return zerocross;
}

// -----------------------------------------------------------------------------
// Track angle - returns 1 with CW zero cross, -1 with CCW zero cross
// -----------------------------------------------------------------------------
int SPD_TrackAngle(void){
	int zero;
	if (spd.fbSensor == SPD_HALLFB) zero = SPD_TrackHallAngle();
	if (spd.fbSensor == SPD_ENCODERFB) zero = SPD_TrackEncoderAngle();
	if (spd.fbSensor == SPD_SINCOSFB) zero = SPD_TrackSinCosAngle();
	spd.revCount += zero;
	return zero;
}

// -----------------------------------------------------------------------------
// Store hall angle
// -----------------------------------------------------------------------------
void SPD_StoreHallAngleData(int hallAngleIndex){
	int hallCode;
	hallCode = SPD_GetHallCode();
	spd.hallCodeSequence[hallAngleIndex] = hallCode;
}

// -----------------------------------------------------------------------------
// get hall code angle from index (for calibration)
// -----------------------------------------------------------------------------
int SPD_GetHallCodeAngle(int index){
	if ((index >=0) && (index <= 5)){
		return hallAngle[index];
	} else {
		return 0;
	}
}

// -----------------------------------------------------------------------------
// build hall code table
// -----------------------------------------------------------------------------
void SPD_BuildHallCodeTable(void){
	int index, code;
	for (index=0; index<6; index++){
		// use code to enter hallCodeTable
		code = spd.hallCodeSequence[index];
		// code angle index is index
		spd.hallCodeTable[code].codeAngleIndex = index;
		// last CCW code is (index-1) into code sequence
		if (index==0) spd.hallCodeTable[code].lastCCWCode = spd.hallCodeSequence[5];
		else spd.hallCodeTable[code].lastCCWCode = spd.hallCodeSequence[index-1];
		// last CW code is (index+1) into code sequence
		spd.hallCodeTable[code].lastCWCode = spd.hallCodeSequence[index+1];
		// CCW edge index is (index)
		spd.hallCodeTable[code].CCWEdgeIndex = index;
		// CW edge index is (index+1)
		spd.hallCodeTable[code].CWEdgeIndex = index+1;
	}
}

// -----------------------------------------------------------------------------
// Set encoder to angle scaling
// -----------------------------------------------------------------------------
void SPD_SetEncoderToAngleScaling(int16_t value, int16_t fp){
	// includes PPR and motor # poles information
	// = 1/(2*ENCODER_PPR) * (#poles / 2) * ANGLE_360DEG
	spd.encToAngle = value;
	spd.encToAngleFp = fp;
}

// -----------------------------------------------------------------------------
// change sign of encoder to angle - run at calibration
// -----------------------------------------------------------------------------
void SPD_EncoderChangeSign(void){
	spd.encToAngle = -spd.encToAngle;
}

// -----------------------------------------------------------------------------
// Zero angle
// -----------------------------------------------------------------------------
void SPD_ZeroAngle(void){
	ENC_ResetCounter();
}

// -----------------------------------------------------------------------------
// set sensor
// -----------------------------------------------------------------------------
void SPD_SetSensor(int sensor){
	if ((sensor >= 0)&&(sensor < SPD_SENSORCOUNT)) spd.fbSensor = sensor;
}
// -----------------------------------------------------------------------------
// get sensor
// -----------------------------------------------------------------------------
int SPD_GetSensor(void){ return spd.fbSensor; }

// -----------------------------------------------------------------------------
// set revolution count
// -----------------------------------------------------------------------------
void SPD_SetRevCount(int value){
	spd.revCount = value;
}
// -----------------------------------------------------------------------------
// get revolution count
// -----------------------------------------------------------------------------
int SPD_GetRevCount(void){
	return spd.revCount;
}

// -----------------------------------------------------------------------------
// track sin/cos encoder
// -----------------------------------------------------------------------------
int SPD_TrackSinCosAngle(void){
	int16_t pha, phb;
	int zerocross = 0;

	// get values for PHA and PHB
	out1=pha = FB_Get(FB_PHA);
	out2=phb = FB_Get(FB_PHB);

	// calculate new angle
	spd.angle = LIB_Atan2(pha, phb);

	// check for zero cross both directions
	if ((spd.angle-spd.lastAngle)>ANGLE_180DEG) zerocross = -1;
	if ((spd.lastAngle-spd.angle)>ANGLE_180DEG) zerocross = 1;

	// save angle
	spd.lastAngle = spd.angle;

	// return zerocross
	return zerocross;
}


