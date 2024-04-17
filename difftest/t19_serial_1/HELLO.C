#include <REG52.H> 
#include <stdio.h> 
#include <string.h>

void uart_init(void) {
    SCON = 0x50;           // 8-bit variable baud rate; receiver enabled
    TMOD |= 0x20;          // Timer1 mode 2, 8-bit auto-reload
    TH1 = 0xFD;            // 9600 baud rate with 11.0592MHz oscillator
    TR1 = 1;               // Start timer1
    TI = 1;                // Clear transmit interrupt flag
    RI = 0;                // Clear receive interrupt flag
}

void uart_write(char c) {
    SBUF = c;
    while(TI == 0);        // Wait for transmission to complete
    TI = 0;                // Clear transmission interrupt flag
}

char uart_read(void) {
    while(RI == 0);        // Wait until character received
    RI = 0;                // Clear receive interrupt flag
    return SBUF;           // Return character from serial buffer
}

void main(void) {
    uart_init();           // Initialize UART settings

    while (1) {
        char c = uart_read(); // Read character
        uart_write(c);     // Echo character back

        if (c == '\r') {
            uart_write('\n'); // If carriage return, also send new line
        }
    }
}


