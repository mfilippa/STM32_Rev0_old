//------------------------------------------------------------------------------
// Global definitions
//------------------------------------------------------------------------------

#ifndef _GLOBALS_H    // define header to include only once
#define _GLOBALS_H

// global debug variables
#ifndef MATLAB_SIMULATION
extern int16_t inp1, inp2, inp3;
extern int16_t out1, out2, out3;
#else
extern double inp1, inp2, inp3;
extern double out1, out2, out3;
#endif

#define FP_PU			12  // [pu]   -8 to 8 pu
#define FP_VOLTAGE       6	// [V]  -512 to 512 V
#define FP_CURRENT		10  // [A] 	 -32 to 32 A
#define FP_AMPLITUDE 	14  // [1]    -2 to 2
#define FP_FREQUENCY 	 6	// [Hz] -512 to 512 Hz 
#define FP_ANGLE		11  // [rad] -16 to 16 rad (-5pi to 5pi)
#define FP_TEMPERATURE   7  // [C]  -256 to 256
#define FP_STEPTIME		24  // [s] ~-2ms to 2ms
#define FP_LOGLUX		12  // [s]   ~-8 to 8 = 100Mlux to 10nlux

#define ANGLE_0DEG		0
#define ANGLE_30DEG		1072
#define ANGLE_60DEG		2144
#define ANGLE_90DEG		3216
#define ANGLE_120DEG	4289
#define ANGLE_150DEG	5361
#define ANGLE_180DEG	6433
#define ANGLE_210DEG	7506
#define ANGLE_240DEG	8578
#define ANGLE_270DEG	9650
#define ANGLE_300DEG	10723
#define ANGLE_330DEG	11795
#define ANGLE_360DEG	12867	// angle range goes to this value inclusive

#define FP1			1
#define FP2			2
#define FP3			3
#define FP4			4
#define FP5			5
#define FP6			6
#define FP7			7
#define FP8			8
#define FP9			9
#define FP10		10
#define FP11		11
#define FP12		12
#define FP13		13
#define FP14		14
#define FP15		15
#define FP16		16
#define FP24		24
#define FP27		27

#endif	// _GLOBALS_H

