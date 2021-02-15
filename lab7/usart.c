/*
 * usart.c
 *
 * Created: 4/6/2018 9:21:33 PM
 * Author: Wes Piard
 * Modified By: Chris Crary, Dr. Schwartz
 * Last Modified: 23 Oct, 2019
 */

#include "usart.h"

/*****************************************************************************
* Name: usartd0_init
* Purpose: To configure the USARTD0 module for a specific asynchronous
*          serial protocol, and to enable the corresponding receiver and
*          transmitter.
* Input(s): N/A
* Output: N/A
******************************************************************************/
void usartd0_init(void)
{
  /* configure relevant TxD and RxD pins */
	PORTD.OUTSET = PIN3_bm;
	PORTD.DIRSET = PIN3_bm;
	PORTD.DIRCLR = PIN2_bm;

  /* configure baud rate */
	/* At 2 MHz SYSclk, 5 BSEL, -6 BSCALE corresponds to 115200 bps */
	USARTD0.BAUDCTRLA = (uint8_t)11;
	USARTD0.BAUDCTRLB = (uint8_t)( (-7 << 4) | (11>> 8) );

  /* configure remainder of serial protocol */
  /* in this example, a protocol with 8 data bits, no parity, and
   * one stop bit is chosen. */
	USARTD0.CTRLC = USART_CMODE_ASYNCHRONOUS_gc |
					        USART_PMODE_DISABLED_gc		  |
					        USART_CHSIZE_8BIT_gc        &
                  ~USART_SBMODE_bm;

  /* enable receiver and/or transmitter systems */
	USARTD0.CTRLB = USART_RXEN_bm | USART_TXEN_bm;

  /* enable interrupt (optional) */
	USARTD0.CTRLA = USART_RXCINTLVL_LO_gc; 
}

/*****************************************************************************
* Name: usartd0_out_char
* Purpose: To output a character from the transmitter within USARTD0.
* Input(s): c (char)
* Output: N/A
******************************************************************************/
void usartd0_out_char(char c)
{
	while(!(USARTD0.STATUS & USART_DREIF_bm));
	USARTD0.DATA = c;
}

/*****************************************************************************
* Name: usartd0_out_string
* Purpose: To output a C string from the transmitter within USARTD0.
* Input(s): str (char *)
* Output: N/A
******************************************************************************/
void usartd0_out_string(char * str)
{
	while(*str) usartd0_out_char(*(str++));
}

/*****************************************************************************
* Name: usartd0_in_char
* Purpose: To read in a character from the receiver within USARTD0.
* Input(s): N/A
* Output: (char)
******************************************************************************/
char usartd0_in_char(void)
{
  /* intentionally left blank */
  while(!(USARTD0.STATUS & USART_RXCIF_bm));    
  
  return USARTD0.DATA;
 
}

/*****************************************************************************
* Name: usartd0_in_string
* Purpose: To read in a string from the receiver within USARTD0. The string
*          is to be stored within a pre-allocated buffer (buf).
* Input(s): buf (char *)
* Output: N/A
******************************************************************************/
void usartd0_in_string(char * buf)
{
  /* intentionally left blank */
}