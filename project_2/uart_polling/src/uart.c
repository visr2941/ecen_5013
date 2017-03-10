#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "uart.h"


#define CLK_DIVIDE 16
#define UART_CLOCK 24000000

void uart_configure(int16_t baudrate)
{
	uint16_t div;

	/*Clocking*/
        SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;	// setting clock for UART0
        SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;	// setting clock for PORTA
        SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1);	// setting maximum clock for UART0

	/*Setting Tx and Rx Pins*/
	PORTA_PCR1 = PORT_PCR_MUX(2);   // SET PTA1 AS RX
    	PORTA_PCR2 = PORT_PCR_MUX(2);	// SET PTA1 AS TX
	
	UART0_C1 |= 0x00;
        UART0_C2 |= (UART0_C2_TE_MASK|UART0_C2_RE_MASK);
	/*Getting the correct baudrate for the UART0*/
	div = (UART_CLOCK/CLK_DIVIDE)/baudrate-1;	// getting the div value to set the baud rate for UART0
	UART0_BDH = (div >> 8) & UART0_BDH_SBR_MASK;	// setting the higher byte 
    	UART0_BDL = (div & UART0_BDL_SBR_MASK);		// setting the lower byte

	/*Configuring UART0 control register*/
        UART0_C1 |= 0x00;
        UART0_C2 |= (UART0_C2_TE_MASK|UART0_C2_RE_MASK);	// enabling Tx & Rx
        UART0_C3 |= 0x00;
        UART0_C4 |= UART0_C4_OSR(CLK_DIVIDE-1);			// setting the oversampling ratio
        UART0_C5 |= 0x00;	
	
	//NVIC->ISER[0] = (1<<12);	// enable IRQ for UART0
}

void uart_send_byte(uint8_t data)
{
	while(!(UART0_S1 & UART0_S1_TDRE_MASK));	// waiting till the transmission buffer is empty
	UART0_D = data;					// putting the data into Tx register
	//while(!(UART0_S1 & UART0_S1_TC_MASK));		// blocking till the transmission complete
}

void uart_send_byte_n(uint8_t * dataptr, uint16_t length)
{
	while(length--)				// while length is not zero
		uart_send_byte(*dataptr++);	// keep sending the data
}

uint8_t uart_receive_byte()
{
	while(!(UART0_S1 & UART0_S1_RDRF_MASK));	// loop until the receiver buffer is empty
	return(UART0_D);				// return the data received
}
