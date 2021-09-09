// -----------------------------------------------------------------------------
// Scheduler functions - MPF 08/2011
// -----------------------------------------------------------------------------

// include files
#include "../stdint.h"
#include "../globals.h"
#include "../hal.h"
#include "sch.h"
#include "fault.h"

// definitions
#define SCH_NRFUNCTIONS   2
#define SCH_NRTIMERS	 10
#define MSEC 		    (1)		// when executing at 1kHz
#define SEC          (1000)
#define MIN        (60*SEC)
#define HR         (60*MIN)
#define FLASH_WAIT		 10		// 5sec at 500msec task

//module structure
struct sch_struct {
	// scheduler table structure
	struct schTable_struct{
		int32_t ticks;        // tick frequency to execute scheduled function
		int32_t count;		   // tick countdown
		void (*function) (void);  	// pointer to scheduled function -> void function(void)
	} table[SCH_NRFUNCTIONS];

	// timer structure
	struct schTimer_struct{
		int32_t count;
	} timer[SCH_NRTIMERS];

	// led flash structure
	struct schLed_struct {
		int flashCode;
		int flashCount;
		int flashWait;
	} led;
} sch;

// -----------------------------------------------------------------------------
// Scheduler tick
// -----------------------------------------------------------------------------
void SCH_Step(void){
	int i;
	// decrement tick count for every scheduled function, if timer expired,
	// execute scheduled function
    for (i=0; i< SCH_NRFUNCTIONS; i++){
        if (sch.table[i].count!= 0) sch.table[i].count--;
		else {
            sch.table[i].count = (sch.table[i].ticks-1);
            sch.table[i].function();
		}
    }
}

// -----------------------------------------------------------------------------
// dummy task
// -----------------------------------------------------------------------------
void SCH_DummyTask(void){

}

// -----------------------------------------------------------------------------
// set timer
// -----------------------------------------------------------------------------
void SCH_SetTimer(int index, int32_t ms){
	sch.timer[index].count = ms;
}

// -----------------------------------------------------------------------------
// Check if timer has expired - returns 1 if expired
// -----------------------------------------------------------------------------
int SCH_TimerHasExpired(int index){
	return (sch.timer[index].count == 0);
}

// -----------------------------------------------------------------------------
// decrement timers
// -----------------------------------------------------------------------------
void SCH_DecrementTimers(void){
	int index;
	// cycle through timers decrement those not expired
	for (index=0; index<SCH_NRTIMERS; index++){
		if (sch.timer[index].count != 0) sch.timer[index].count--;
	}
}

// -----------------------------------------------------------------------------
// Flash manager
// -----------------------------------------------------------------------------
void SCH_FlashManager(void){
	uint16_t fault;
	int index;
	
	// search for highest priority fault and set fault count/wait
	fault = FAULT_Get();
	if (fault != 0) {
		for (index = 0; index < 16; index++){
			if ((fault & (1<<index)) != 0) 	{
				// fault found, store code and exit loop
				sch.led.flashCode = (index+1);
				break;
			}
		}
	} else {
		// clear codes and wait count
		sch.led.flashCode = 0;
		sch.led.flashWait = FLASH_WAIT;
	}
	
	// flash LED normally if there is no code
	if (sch.led.flashCode == 0) {
        IO_Toggle(IO_LED);
	} else {	
		// else flash error code
        if(sch.led.flashWait != 0) {
        	// wait state
            sch.led.flashWait--;
			IO_Reset(IO_LED);
        } else {
        	// set count after coming out of a wait state
			if (sch.led.flashCount == 0) {
				sch.led.flashCount = sch.led.flashCode << 1;
				IO_Reset(IO_LED);
			}

			// flash until count reaches zero
			IO_Toggle(IO_LED);
			sch.led.flashCount--;
			if(sch.led.flashCount == 0) {
				sch.led.flashWait = FLASH_WAIT;
				IO_Reset(IO_LED);
			}
        }
	}
}

// -----------------------------------------------------------------------------
// initialize module
// -----------------------------------------------------------------------------
void SCH_Initialize(void){
	int index;

    // initialize scheduler table
	sch.table[0].ticks    = 1*MSEC;
	sch.table[0].count    = 0;
	sch.table[0].function = &SCH_DecrementTimers;

	sch.table[1].ticks    = 500*MSEC;
	sch.table[1].count    = 0;
	sch.table[1].function = &SCH_FlashManager;

	// initialize scheduler timers
	for (index=0; index<SCH_NRTIMERS; index++){
		sch.timer[index].count = 0;
	}

	// initialize led flash
	sch.led.flashCode = 0;
}
