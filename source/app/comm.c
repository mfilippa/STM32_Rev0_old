// -----------------------------------------------------------------------------
// Communications - MPF 08/2011
// -----------------------------------------------------------------------------

// includes
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "lib.h"
#include "comm.h"
#include "fault.h"
#include "ctrl.h"
#include "acdr.h"
#include "bldc.h"
#include "dpwm.h"
#include "linear.h"

// defines
#define DBG_BUFFER_SIZE		512

// global debug variables - used for debug purposes only
#ifndef MATLAB_SIMULATION
int16_t inp1, inp2, inp3;
int16_t out1, out2, out3;
#endif

// comm structure
struct comm_struct 	{
	int16_t buffer1[DBG_BUFFER_SIZE];
	int16_t buffer2[DBG_BUFFER_SIZE];
	int16_t buffer3[DBG_BUFFER_SIZE];
	int16_t buffer4[DBG_BUFFER_SIZE];
	int16_t buffer5[DBG_BUFFER_SIZE];
	int16_t buffer6[DBG_BUFFER_SIZE];
	int index;
	int enableBufferWrite;
	int downsampleCount;
	int downsampleValue;
} comm;
	
// -----------------------------------------------------------------------------
// module initialization
// -----------------------------------------------------------------------------
void COMM_Initialize(void){
	
	SERIAL_Initialize();
	
	// initialize debug variables
	inp1 = inp2 = inp3 = out1 = out2 = out3 = 0 ;
	for (comm.index = 0; comm.index < DBG_BUFFER_SIZE; comm.index++) {
		comm.buffer1[comm.index] = comm.buffer2[comm.index] = 
			comm.buffer3[comm.index] = 0;
	}
	comm.enableBufferWrite = 0;		
	comm.downsampleCount = comm.downsampleValue = 0;

	// send zero to indicate communications has been initialized
	SERIAL_WriteByte(0);
}

// -----------------------------------------------------------------------------
// enable write to buffer
// -----------------------------------------------------------------------------
void COMM_EnableBufferWrite(void){
	comm.index = 0;
	comm.enableBufferWrite = 1;
}

// -----------------------------------------------------------------------------
// disable write to buffer
// -----------------------------------------------------------------------------
void COMM_DisableBufferWrite(void){
	comm.index = 0;
	comm.enableBufferWrite = 0;
}

// -----------------------------------------------------------------------------
// set downsample value
// -----------------------------------------------------------------------------
void COMM_SetDownsampleValue(int value){
	comm.downsampleValue = value;
}

// -----------------------------------------------------------------------------
// communication step
// -----------------------------------------------------------------------------
void COMM_Execute(void){
	int command, tmp;
	
	// forever loop
	while(1){
	
		command = SERIAL_ReadByte();
		
		// comm state machine
		switch (command){
			// -----------------------------------------------------------------
			case 0xA0:
				tmp = SERIAL_ReadByte();
				if (tmp == 0) IO_Reset(IO_INRUSH);
				else IO_Set(IO_INRUSH);
				break;
			case 0xA1:
				tmp = SERIAL_ReadByte();
				if (tmp == 0) IO_Reset(IO_BEMF_TRANSISTOR);
				else IO_Set(IO_BEMF_TRANSISTOR);
				break;
			case 0xA2:
				tmp = SERIAL_ReadByte();
				if (tmp == 0) CTRL_SetGateDriveEnableState(0);
				else CTRL_SetGateDriveEnableState(1);
				break;
			case 0xA3:
				PWM_SetOutputState(SERIAL_ReadByte());
				break;
			case 0xA4:
				tmp = SERIAL_ReadWord();
				PWM_Set(tmp, tmp, tmp);
				break;
			case 0xA5:
				PWM_DisableChannel(SERIAL_ReadByte());
				break;
			case 0xA6:
				PWM_Set(SERIAL_ReadWord(), 0, 0);
				COMM_EnableBufferWrite();
				break;
			case 0xA7:
				PWM_Set(0, SERIAL_ReadWord(), 0);
				COMM_EnableBufferWrite();
				break;
			case 0xA8:
				PWM_Set(0, 0, SERIAL_ReadWord());
				COMM_EnableBufferWrite();
				break;
			// -----------------------------------------------------------------
			case 0xB0:
				CTRL_SetSetpoint((int16_t)SERIAL_ReadWord());
				COMM_EnableBufferWrite();
				break;
			case 0xB1:
				CTRL_StartStop(SERIAL_ReadByte());
				break;
			case 0xB2:
				CTRL_SetAmplitude((int16_t)SERIAL_ReadWord());
				COMM_EnableBufferWrite();
				break;
			case 0xB3:
				CTRL_SetFrequency((int16_t)SERIAL_ReadWord());
				COMM_EnableBufferWrite();
				break;
			case 0xB4:
				CTRL_SetDirection((int16_t)SERIAL_ReadWord());
				break;
			case 0xB5:
				CTRL_SetMode(SERIAL_ReadByte());
				break;
			case 0xB6:
				ACDR_SetAngle((int16_t)SERIAL_ReadWord());
				COMM_EnableBufferWrite();
				break;
			case 0xB7:
				CTRL_CalibrateSensor();
				break;
			case 0xB8:
				BLDC_SetAdvanceAngle((int16_t)SERIAL_ReadWord());
				break;
			case 0xB9:
				CTRL_SetSensor(SERIAL_ReadByte());
				break;
			case 0xBA:
				DPWM_SetMode(SERIAL_ReadByte());
				break;
			case 0xBB:
				SPD_ZeroAngle();
				break;
			case 0xBC:
				tmp = SERIAL_ReadByte();
				if (tmp==0) LINEAR_SetPosAngle(SERIAL_ReadWord());
				if (tmp==1) LINEAR_SetNegAngle(SERIAL_ReadWord());
				if (tmp==2) LINEAR_SetPosEndAngle(SERIAL_ReadWord());
				if (tmp==3) LINEAR_SetNegEndAngle(SERIAL_ReadWord());
				break;
			// -----------------------------------------------------------------
			case 0xD0:
				SERIAL_WriteByte(command);
				// send all feedbacks
				SERIAL_WriteWord(FB_Get(FB_IA));
				SERIAL_WriteWord(FB_Get(FB_IB));
				SERIAL_WriteWord(FB_Get(FB_IC));
				SERIAL_WriteWord(FB_Get(FB_VA));
				SERIAL_WriteWord(FB_Get(FB_VB));
				SERIAL_WriteWord(FB_Get(FB_VC));
				SERIAL_WriteWord(FB_Get(FB_VBUS));
				SERIAL_WriteWord(FB_Get(FB_HST));
				// send fault code
				SERIAL_WriteWord(FAULT_Get());
				// send debug variables
				SERIAL_WriteWord(out1);
				SERIAL_WriteWord(out2);
				SERIAL_WriteWord(out3);
				// send hall state
				SERIAL_WriteWord(SPD_GetAngle());
				SERIAL_WriteWord(SPD_GetHallCode());
				SERIAL_WriteWord(SPD_GetSpeed());
				// send qd0 data
				SERIAL_WriteWord(qd0.q);
				SERIAL_WriteWord(qd0.d);
				break;
			case 0xD1:
				// send buffers if not writing to them
				if (!comm.enableBufferWrite){
					for (comm.index=0; comm.index<DBG_BUFFER_SIZE; comm.index++){
						SERIAL_WriteWord(comm.buffer1[comm.index]);
					}
					for (comm.index=0; comm.index<DBG_BUFFER_SIZE; comm.index++){
						SERIAL_WriteWord(comm.buffer2[comm.index]);
					}
					for (comm.index=0; comm.index<DBG_BUFFER_SIZE; comm.index++){
						SERIAL_WriteWord(comm.buffer3[comm.index]);
					}
					for (comm.index=0; comm.index<DBG_BUFFER_SIZE; comm.index++){
						SERIAL_WriteWord(comm.buffer4[comm.index]);
					}
					for (comm.index=0; comm.index<DBG_BUFFER_SIZE; comm.index++){
						SERIAL_WriteWord(comm.buffer5[comm.index]);
					}
					for (comm.index=0; comm.index<DBG_BUFFER_SIZE; comm.index++){
						SERIAL_WriteWord(comm.buffer6[comm.index]);
					}
				} else {
					SERIAL_WriteByte(0xFF);
				}
				break;
			case 0xD2:
				// send debug inputs/outputs
				SERIAL_WriteWord(inp1);
				SERIAL_WriteWord(inp2);
				SERIAL_WriteWord(inp3);
				SERIAL_WriteWord(out1);
				SERIAL_WriteWord(out2);
				SERIAL_WriteWord(out3);
				break;
			case 0xD3:
				tmp = SERIAL_ReadByte();
				// get debug inputs
				if (tmp == 1) inp1 = SERIAL_ReadWord();
				else if (tmp == 2) inp2 = SERIAL_ReadWord();
				else if (tmp == 3) inp3 = SERIAL_ReadWord();
				break;
			case 0xD4:
				// enable buffer writes
				COMM_EnableBufferWrite();
				break;
			case 0xD5:
				// disable buffer writes
				COMM_DisableBufferWrite();
				break;
			case 0xD6:
				// set downsampling
				COMM_SetDownsampleValue(SERIAL_ReadWord());
				break;
			// -----------------------------------------------------------------
			case 0xF0:
				// send faults
				SERIAL_WriteWord(FAULT_Get());
				break;
			case 0xF1:
				// clear faults
				FAULT_Clear(SERIAL_ReadWord());
				break;
			// -----------------------------------------------------------------
			default:
				// command not recognized
				command = 0xFF;
				break;
		}
		
		// acknowledge command
		SERIAL_WriteByte(command);
	}
}

// -----------------------------------------------------------------------------
// save to buffer
// -----------------------------------------------------------------------------
void COMM_SaveToBuffer(int bufferIndex, int16_t value){
	// only writes to buffer #6 increment index. If less than 6 buffers are
	// needed, always write to #6 last
	if (comm.enableBufferWrite){
		switch (bufferIndex){
			case 1:
				comm.buffer1[comm.index] = value;
			break;
			case 2:
				comm.buffer2[comm.index] = value;
			break;
			case 3:
				comm.buffer3[comm.index] = value;
			break;
			case 4:
				comm.buffer4[comm.index] = value;
			break;
			case 5:
				comm.buffer5[comm.index] = value;
			break;
			case 6:
				comm.buffer6[comm.index] = value;
				// advance buffer index if downsample count has reached zero
				if (comm.downsampleCount!=0) comm.downsampleCount--;
				else {
					comm.index++;
					if (comm.index >= DBG_BUFFER_SIZE) comm.enableBufferWrite = 0;
					comm.downsampleCount = comm.downsampleValue;
				}
			break;
		}
	}
}


