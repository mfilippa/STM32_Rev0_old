//------------------------------------------------------------------------------
// Control - MPF 08/2011
//------------------------------------------------------------------------------

// include files
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "sch.h"
#include "fault.h"
#include "ctrl.h"
#include "bldc.h"
#include "acdr.h"
#include "dpwm.h"
#include "lib.h"
#include "comm.h"
#include "linear.h"


// module structure
struct ctrl_struct {
	int mode;
	int16_t setpoint;
	int16_t frequency;
	int16_t amplitude;
	int direction;
	void (*ControlFunction)(void);
	int state;
	int32_t waitcount;
	int nextstate;
} ctrl;

// global dq monitor variable
struct lib_abcqd0struct qd0;		// monitor/debug variable in abc/qd0

// prototypes
void CTRL_SetMode(int mode);

//------------------------------------------------------------------------------
// initialize control
//------------------------------------------------------------------------------
void CTRL_Initialize(void){

	// initialize variables
	ctrl.setpoint = 0;
	ctrl.frequency = 0;
	ctrl.amplitude = 0;
	ctrl.direction = CTRL_DIR_CW;
	ctrl.state = CTRL_IDLE;

	// initialize control mode
	CTRL_SetMode(CTRL_ACDRIVE_OL);

	// initialize back emf transistor to ON state
	IO_Set(IO_BEMF_TRANSISTOR);

}

//------------------------------------------------------------------------------
// get control state
//------------------------------------------------------------------------------
int CTRL_GetState(void) { return ctrl.state; }

//------------------------------------------------------------------------------
// get control frequency
//------------------------------------------------------------------------------
int16_t CTRL_GetFrequency(void) { return ctrl.frequency; }

//------------------------------------------------------------------------------
// get control amplitude
//------------------------------------------------------------------------------
int16_t CTRL_GetAmplitude(void) { return ctrl.amplitude; }

//------------------------------------------------------------------------------
// get control direction
//------------------------------------------------------------------------------
int CTRL_GetDirection(void) { return ctrl.direction; }

//------------------------------------------------------------------------------
// get control mode
//------------------------------------------------------------------------------
int CTRL_GetMode(void) {return ctrl.mode; }

//------------------------------------------------------------------------------
// get setpoint
//------------------------------------------------------------------------------
int16_t CTRL_GetSetpoint(void) {return ctrl.setpoint; }

//------------------------------------------------------------------------------
// set setpoint
//------------------------------------------------------------------------------
void CTRL_SetSetpoint(int16_t sp){
	ctrl.setpoint = sp;
}

//------------------------------------------------------------------------------
// set amplitude
//------------------------------------------------------------------------------
void CTRL_SetAmplitude(int16_t amp){
	if (amp > (1<<FP_AMPLITUDE)) ctrl.amplitude = (1<<FP_AMPLITUDE);
	else ctrl.amplitude = amp;
}

//------------------------------------------------------------------------------
// set frequency
//------------------------------------------------------------------------------
void CTRL_SetFrequency(int16_t freq){
	ctrl.frequency = freq;
}

//------------------------------------------------------------------------------
// set direction
//------------------------------------------------------------------------------
void CTRL_SetDirection(int direction){
	if (direction == CTRL_DIR_CW) ctrl.direction = CTRL_DIR_CW;
	else if (direction == CTRL_DIR_CCW) ctrl.direction = CTRL_DIR_CCW;
}

//------------------------------------------------------------------------------
// set sensor type
//------------------------------------------------------------------------------
void CTRL_SetSensor(int sensor){
	if (ctrl.state == CTRL_IDLE) {
		SPD_SetSensor(sensor);
	}
}

//------------------------------------------------------------------------------
// set control mode
//------------------------------------------------------------------------------
void CTRL_SetMode(int mode){
	if ((mode >=0)&&(mode < CTRL_MODECOUNT)){
		// set mode and function pointer
		ctrl.mode = mode;
		switch (ctrl.mode){
		case CTRL_ACDRIVE_OL: case CTRL_ACDRIVE_VHZ:
			ctrl.ControlFunction = &ACDR_Step;
			break;
		case CTRL_BLDC_VECTOR_VOL: case CTRL_BLDC_COMM_VOL:
		case CTRL_BLDC_VECTOR_ICL: case CTRL_BLDC_COMM_ICL:
			ctrl.ControlFunction = &BLDC_Step;
			break;
		case CTRL_LINEAR:
			ctrl.ControlFunction = &LINEAR_Step;
			break;
		}
	}
}

//------------------------------------------------------------------------------
// enable or disable gate driver
//------------------------------------------------------------------------------
void CTRL_SetGateDriveEnableState(int state){
	if (state == 0){
		// disable
		// sequence: kill PWMs, disable BRK interrupts (to avoid generating a
		// fault) when SD/OC pin goes low, then disable GD
		PWM_SetOutputState(0);
		PWM_SetBreakIRQEnableState(0);
		IO_Reset(IO_GATEDRIVE);
	} else{
		// enable
		IO_Set(IO_GATEDRIVE);
	}
}

//------------------------------------------------------------------------------
// control fast step
//------------------------------------------------------------------------------
void CTRL_FastStep(void){

	// track angle
	SPD_TrackAngle();

	switch (ctrl.state){

	// control idle state --------------------------------------------------
	case CTRL_IDLE:
		// do nothing
		break;

		// start control -------------------------------------------------------
	case CTRL_START:
		// set initial pwm to 0%, enable GD and PWMs
		PWM_Set(0, 0, 0);
		CTRL_SetGateDriveEnableState(1);
		PWM_DisableChannel(PWM_NONE);
		// initialize modules if needed
		if (ctrl.mode == CTRL_LINEAR) LINEAR_Initialize();
		// wait to enable PWM outputs
		ctrl.waitcount = CTRL_STEP_FREQ/20;			// 50ms
		ctrl.nextstate = CTRL_PWMON;
		ctrl.state = CTRL_WAIT;
		break;

		// enable PWMs, precharge bootstrap caps -------------------------------
	case CTRL_PWMON:
		// enable PWMs
		PWM_SetOutputState(1);
		// wait to precharge boostraps
		ctrl.waitcount = CTRL_STEP_FREQ/20;			// 50ms
		ctrl.nextstate = CTRL_RUN;
		ctrl.state = CTRL_WAIT;
		break;

		// run control ---------------------------------------------------------
	case CTRL_RUN:
		// execute control step
		ctrl.ControlFunction();
		break;

		// stop control --------------------------------------------------------
	case CTRL_STOP:
		// disable gate drivers and PWM outputs, switch to IDLE state
		CTRL_SetGateDriveEnableState(0);
		PWM_SetOutputState(0);
		ctrl.state = CTRL_IDLE;
		break;

		// wait state ----------------------------------------------------------
	case CTRL_WAIT:
		// countdown, switch to next state if expired
		ctrl.waitcount--;
		if (ctrl.waitcount <=0) ctrl.state = ctrl.nextstate;
		break;

		// state not recognized ------------------------------------------------
	default:
		// default to stop state
		ctrl.state = CTRL_STOP;
		break;
	}

	// write to buffers
//	COMM_SaveToBuffer(1,ctrl.qd0.a);
//	COMM_SaveToBuffer(2,ctrl.qd0.b);
//	COMM_SaveToBuffer(3,ctrl.qd0.c);
//	COMM_SaveToBuffer(4,ctrl.qd0.q);
//	COMM_SaveToBuffer(5,ctrl.qd0.d);
//	COMM_SaveToBuffer(6,ctrl.qd0.angle);
}

//------------------------------------------------------------------------------
// control slow step
//------------------------------------------------------------------------------
void CTRL_SlowStep(void){
	// monitor faults, stop control if fault detected
	if ((FAULT_Get() != 0) && (ctrl.state != CTRL_IDLE)){
		CTRL_StartStop(0);
	}
}

//------------------------------------------------------------------------------
// start/stop control
//------------------------------------------------------------------------------
void CTRL_StartStop(int value){
	switch (value){
	case 0:
		if (ctrl.state != CTRL_IDLE) ctrl.state = CTRL_STOP;
		break;
	case 1:
		if ((ctrl.state == CTRL_IDLE) && (FAULT_Get() == FAULT_NONE)){
			ctrl.state = CTRL_START;
		}
		break;
	}
}

//------------------------------------------------------------------------------
// Calibrate BLDC Hall sensor - call from background loop !!
//------------------------------------------------------------------------------
void CTRL_CalibrateSensor(void){

	CTRL_SetMode(CTRL_ACDRIVE_OL);
	CTRL_SetFrequency(0);
	CTRL_StartStop(1);

	// hall sensor calibration
	if (SPD_GetSensor() == SPD_HALLFB) {
		int index;
		for (index=0; index<6; index++){
			// set angle, wait for timeout, write data to table
			// repeat for all 6 states
			ACDR_SetAngle(SPD_GetHallCodeAngle(index));
			SCH_SetTimer(0,1000);
			while (SCH_TimerHasExpired(0)==0);
			SPD_StoreHallAngleData(index);
		}
		// build table
		SPD_BuildHallCodeTable();
	}

	// encoder calibration - aligns 'q' axis to zero by driving 'd' with acdr
	if (SPD_GetSensor() == SPD_ENCODERFB){
		// calibrate to zero degrees, wait for shaft alignment
		ACDR_SetAngle(ANGLE_0DEG);
		SCH_SetTimer(0,3000);
		while (SCH_TimerHasExpired(0)==0);
		SPD_ZeroAngle();
		SPD_SetRevCount(0);
		// check sign, correct if necessary
		ACDR_SetAngle(ANGLE_90DEG);
		SCH_SetTimer(0,2000);
		while (SCH_TimerHasExpired(0)==0);
		if (SPD_GetAngle()>ANGLE_180DEG) SPD_EncoderChangeSign();
	}

	CTRL_StartStop(0);
}

