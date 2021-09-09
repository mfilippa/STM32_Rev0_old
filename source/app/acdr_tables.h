// -----------------------------------------------------------------------------
// BLDC tables h file - MPF 08/2011
// -----------------------------------------------------------------------------

#ifndef ACDR_TABLES_H
#define ACDR_TABLES_H

// bldc V/Hz table
#define ACDR_VHZ_TABLE_SIZE       4
const struct lib_tlookupstruct acdr_vhztable[ACDR_VHZ_TABLE_SIZE] ={
	{ 0,0,0,12 },			// 0, 0
	{ 319,0,0,12 }, 		// 5, 0
	{ 320,1638,1906,12},  	// 5, 0.1 }
	{ 3840,3276,0,12},  	// 60, 0.2 }
};

#endif
