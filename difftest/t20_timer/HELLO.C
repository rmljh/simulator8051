#include <REG52.H> 
#include <stdio.h> 
#include <string.h>

void send_msg (char * msg) {
	while (*msg != '\0') {
		SBUF=*msg++;
		while(TI == 0);
		TI=0;
	}
}

static char msg[16];


void main (void) {
	int count = 0;

	SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
	TMOD |= 0x20;               /* TMOD: timer 1, mode 2, 8-bit reload        */
	TH1   = 221;                /* TH1:  reload value for 1200 baud @ 16MHz   */
	TR1   = 1;                  /* TR1:  timer 1 run                          */

	TMOD |= 0x1;  	// T0 = 16bit counter
	TL0   = 0x10; 
	TH0   = 0x10;
	TR0   = 1;

    while (1) {
    while (!TF0) {}
    TF0 = 0;
    TL0   = 0x10; 
    TH0   = 0x10;
		
    sprintf (msg, "Hello %d\n", count++); 
		send_msg(msg);
  }
}


