#include <msp430.h> 

// Turn on Debug modus over UART
#define DEBUG 1

// If not in Debug mode, no serial connection needed
#ifdef DEBUG
 #include "Serial.h"
#endif

// Time for one Bit
#define BITTIME 10000
// LSB (1) or MSB (0) first 
#define LSBFIRST 1
// The Speed of the UARRT connection
#define SERIALSPEED 9600
// The code that is send over IR
#define CODE 240
// Smallest delay between two transmissions 
#define DELAY_BETWEEN_TWO_SENDS 100000
// The clockSpeed in MHZ (CAUSION: needs to be changed in main as well)
#define CLK_SPEED 1
// The PWM frequency in kHz
#define PWM_FREQUZENCY 38
// The PWM periode
#define PWM_PERIODE (CLK_SPEED*1000.0/PWM_FREQUZENCY)

/*
 * main.c
 */


/*
 * Init the Ports
 */
void initPorts() {
	// Set LED1 to OUTPUT and to 0
	P1DIR |= BIT0;
	P1OUT &= ~BIT0;
	
	// Set Push Button to INPUT and enable PULLUPS
	P1DIR &= ~BIT3;
	P1REN |= BIT3;
	
	// Set IROutput PIN to Ouput and disable 
	P1DIR |= BIT6;
	// Set output source to timer A
	P1SEL |= BIT6;
	P1OUT &= ~BIT6;
	
	// PWM setup
	// Set the PWM periode to 38kHz
	TA0CCR0 = PWM_PERIODE;
	// Set dutyCycles to 50%
    TA0CCR1 = PWM_PERIODE/2;
    // Set output mode to resete/set
	TA0CCTL1 |= OUTMOD_7;
	// Set the counter to "count up" to TA0CCR0 with clock input "SMCLK"
	TA0CTL |= TASSEL_2 + MC_1;

}


/*
 * Sends a Code as a Byte by disabling and enabling PWM at the Port
 */
void sendCode(int Byte) {
	// Loop variable
	int i = 0;
	// Latch in Byte so it can be safely modified
	int code = 0;
	// Boolean used for parity check
	int parity = 0;
	if (LSBFIRST) {
		code = Byte;
	} else {
		code = ~Byte;
	}

	// Send start Bit
	P1SEL |= BIT6;
	_delay_cycles(BITTIME/2);

	if (DEBUG) serialPrintInt(1);
	
	// Loop over all Bits
	for (i = 0; i < 8; i++) {
		// If code and Mask not 0
		if ((code & BIT0) > 0) {
			// Send a logical 1
			P1SEL |= BIT6;
			if (DEBUG) serialPrintInt(1);
			// Xor of the Parity Bit
			parity ^= BIT0;
		} else {
			// Send a logical 0
			if (DEBUG) serialPrintInt(0);
			P1SEL &= ~BIT6;
			P1OUT &= ~BIT6;
		}
		// Shift the Bit to send
		code = code >> 1;
		// Wait a Bitlength
		_delay_cycles(BITTIME);
	}

	if (DEBUG) serialPrintInt(parity);
	// If it was an odd number of 1s
	if (parity) {
		// send parity 1
		P1SEL |= BIT6;
	} else {
		// else 0
		P1SEL &= ~BIT6;
		P1OUT &= ~BIT6;
	}
	// Send stop bit
	_delay_cycles(BITTIME);
	if (DEBUG) serialPrintInt(1);
	P1SEL |= BIT6;
	_delay_cycles(BITTIME);
	// Disable PWM on line
	P1SEL &= ~BIT6;
	P1OUT &= ~BIT6;
	if (DEBUG) serialPrintln("");
}


int main(void) {
	// Stop whatchdog
    WDTCTL = WDTPW + WDTHOLD;
	
	// Set clock to 1 MHz
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	

    if (DEBUG) SerialBegin(SERIALSPEED, CLK_SPEED);
    
	// Init the Ports and the timers
    initPorts();

	if (DEBUG) {
		serialPrintln("Init Done!");
		serialPrint("CLK at: ");
		serialPrintInt(CLK_SPEED);
		serialPrintln(" MHz");
		serialPrint("PWM at: ");
		serialPrintInt(PWM_FREQUZENCY);
		serialPrintln(" kHz");
		serialPrint("Periode: ");
		serialPrintInt(PWM_PERIODE);
		serialPrintln(" clkCycles");
	}

	// Do forever and for always
    while(1) {
		// If Push Button pressed
    	if (!(P1IN & BIT3)) {
			// Send specifid Code
    		sendCode(CODE);
			// Display sending with LED on
			P1OUT |= BIT0;
			// Wait so that no Burst can be sended
    		_delay_cycles(DELAY_BETWEEN_TWO_SENDS);
		} else {
			// Turn LED off
			P1OUT &= ~BIT0;
		}
    }
}


