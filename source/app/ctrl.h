//------------------------------------------------------------------------------
// Control .h file - MPF 03/2011
//------------------------------------------------------------------------------

#ifndef CTRL_H    		// define header to include only once
#define CTRL_H

//! \addtogroup app
//! \{
//! \addtogroup ctrl
//! \{

// exported definitions
#define CTRL_DIR_CW	1		// clockwise direction
#define CTRL_DIR_CCW -1		// counterclockwise direction
#define CTRL_STEP_FREQ		8000

// global dq monitor variable
extern struct lib_abcqd0struct qd0;

// exported enums
enum ctrl_state {
	CTRL_IDLE = 0,
	CTRL_START,
	CTRL_PWMON,
	CTRL_RUN,
	CTRL_STOP,
	CTRL_WAIT,
	CTRL_TEMP
};

enum ctrl_modes {
	CTRL_ACDRIVE_OL = 0,		// open loop amp/freq drive
	CTRL_ACDRIVE_VHZ,			// open loop freq/VHz table drive
	CTRL_BLDC_VECTOR_VOL,		// brushless vector + encoder + voltage open loop
	CTRL_BLDC_COMM_VOL,			// brushless commutation + hall + voltage open loop
	CTRL_BLDC_VECTOR_ICL,		// dq vector control + encoder + closed loop current
	CTRL_BLDC_COMM_ICL,			// brushless commutation + hall + closed loop current
	CTRL_LINEAR,				// linear control
	CTRL_MODECOUNT};

// exported functions
extern  void CTRL_Initialize(void);
extern  void CTRL_FastStep(void);
extern  void CTRL_SlowStep(void);

extern  void CTRL_StartStop(int value);
extern  void CTRL_SetSetpoint(int16_t sp);
extern  void CTRL_SetAmplitude(int16_t amp);
extern  void CTRL_SetFrequency(int16_t freq);
extern  void CTRL_SetDirection(int dir);
extern  void CTRL_SetMode(int mode);
extern  void CTRL_SetSensor(int sensor);
extern  void CTRL_CalibrateSensor(void);
extern void CTRL_SetGateDriveEnableState(int state);

extern   int CTRL_GetState(void);
extern int16_t CTRL_GetFrequency(void);
extern int16_t CTRL_GetAmplitude(void);
extern   int CTRL_GetDirection(void);
extern   int CTRL_GetMode(void);
extern int16_t CTRL_GetSetpoint(void);

extern  void CTRL_CalibrateSensor(void);
extern void CTRL_SetGateDriveEnableState(int state);

//! \}
//! \}

#endif



