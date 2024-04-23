#include <REG52.H> 
#include <stdio.h> 
#include <string.h>

static char msg[16];

void send_msg (char * msg) {
	while (*msg != '\0') {
		SBUF=*msg++;
		while(TI == 0);
		TI=0;
	}
}

static void message(int count) {
	msg[0] = 'H';
	msg[1] = 'e';
	msg[2] = 'l';
	msg[3] = 'l';
	msg[4] = 'o';
	msg[5] = ' ';
	msg[6] = (count % 10) + '0';
	msg[7] = '\n';
}

void main (void) {
	int count = 0;

	SCON  = 0x50;		        /* SCON: mode 1, 8-bit UART, enable rcvr      */
	TMOD |= 0x20;               /* TMOD: timer 1, mode 2, 8-bit reload        */
	TH1   = 221;                /* TH1:  reload value for 1200 baud @ 16MHz   */
	TR1   = 1;                  /* TR1:  timer 1 run                          */

    while (1) {
        memset(msg, 0, sizeof(msg));
				message(count++);
        send_msg(msg);
    }
}


