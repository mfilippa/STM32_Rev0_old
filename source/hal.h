// -----------------------------------------------------------------------------
// hal.h - MPF - Dec 5, 2011
// -----------------------------------------------------------------------------

#ifndef _HAL_H_
#define _HAL_H_

#ifndef MATLAB_SIMULATION

#include "./hal/fb.h"
#include "./hal/io.h"
#include "./hal/irq.h"
#include "./hal/pwm.h"
#include "./hal/serial.h"
#include "./hal/spd.h"
#include "./hal/sys.h"

#else

#include "./matlab/fb.h"
#include "./matlab/io.h"
#include "./matlab/irq.h"
#include "./matlab/pwm.h"
#include "./matlab/serial.h"
#include "./matlab/spd.h"
#include "./matlab/sys.h"

#endif

#endif // _HAL_H_
