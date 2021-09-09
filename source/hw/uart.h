// -----------------------------------------------------------------------------
// UART h file - MPF 08/2011
// -----------------------------------------------------------------------------


#ifndef _UART_H
#define _UART_H

//! \addtogroup hw
//! \{
//! \addtogroup uart
//! \{

// exported functions
extern     void UART_Init(int32_t baudRate);
extern     void UART_WriteByte(uint16_t data);
extern     void UART_WriteWord(uint16_t data);
extern uint16_t UART_ReadByte(void);
extern uint16_t UART_ReadWord(void);

//! \}
//! \}

#endif // UART_H




