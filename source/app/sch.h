// -----------------------------------------------------------------------------
// Scheduler functions h file - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef SCH_H
#define SCH_H

//! \addtogroup app
//! \{
//! \addtogroup sch
//! \{

// exported functions
 extern void SCH_Initialize(void);
 extern void SCH_Step(void);
 extern void SCH_SetTimer(int index, int32_t ms);
extern   int SCH_TimerHasExpired(int index);

//! \}
//! \}

#endif 


