// -----------------------------------------------------------------------------
// Speed / angle feedbacks - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef _SPD_H
#define _SPD_H

//! \addtogroup hal
//! \{
//! \addtogroup spd
//! \{

// exported definitions
enum spd_sensors_enum {
	SPD_HALLFB = 0,
	SPD_ENCODERFB,
	SPD_SENSORCOUNT
};

// exported functions
extern void SPD_Initialize(void);
extern int SPD_TrackAngle(void);
extern int16_t SPD_GetAngle(void);
extern int16_t SPD_GetSpeed(void);
extern void SPD_ZeroAngle(void);
extern void SPD_SetSensor(int sensor);
extern int SPD_GetSensor(void);
extern void SPD_SetRevCount(int value);
extern int SPD_GetRevCount(void);

// hall sensor specific
extern int SPD_GetHallCode(void);
extern int SPD_GetHallCodeAngle(int index);
extern void SPD_StoreHallAngleData(int hallAngleIndex);
extern void SPD_BuildHallCodeTable(void);
extern int SPD_GetHallEdgeIndex(void);

// encoder specific
extern void SPD_SetEncoderToAngleScaling(int16_t value, int16_t fp);
extern void SPD_EncoderChangeSign(void);


//! \}
//! \}

#endif
