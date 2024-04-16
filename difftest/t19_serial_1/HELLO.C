#include <REG52.H> 
#include <stdio.h> 
#include <string.h>

void uart_write (char c) {
    SBUF=c;
    while(TI == 0);
    TI=0;
}

char uart_read() {
    char c;

    while(RI==0);
    c = SBUF;
    RI=0; 

    return c;
}

void main (void) {
	int count = 0;

	SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
	TMOD |= 0x20;               /* TMOD: timer 1, mode 2, 8-bit reload        */
	TH1   = 221;                /* TH1:  reload value for 1200 baud @ 16MHz   */
	TR1   = 1;                  /* TR1:  timer 1 run                          */

    count = SBUF;
    while (1) {
		char c = uart_read();
		uart_write(c);

		if (c == '\r') {
			uart_write('\n');
		}
  }
}


