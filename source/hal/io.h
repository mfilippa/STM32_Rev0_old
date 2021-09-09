// -----------------------------------------------------------------------------
// io.h - MPF - Nov 19, 2011
// -----------------------------------------------------------------------------

#ifndef _IO_H_
#define _IO_H_

//! \addtogroup hal
//! \{
//! \addtogroup io
//! \{

// exported definitions
enum io_enum {
	IO_LED = 0,
	IO_DEBUG,
	IO_INRUSH,
	IO_BEMF_TRANSISTOR,
	IO_GATEDRIVE,
	IO_COUNT
};

// exported functions
extern void IO_Initialize(void);
extern void IO_Set(int io);
extern void IO_Reset(int io);
extern void IO_Toggle(int io);
extern  int IO_ReadInput(int io);
extern  int IO_ReadOutput(int io);

//! \}
//! \}

#endif // _IO_H_
