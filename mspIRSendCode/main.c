/*
 * Benjamin Völker, University of Freiburg
 * mail: voelkerb@me.com
 */

#include <msp430.h> 

// Turn on Debug modus over UART
#define DEBUG 0

// If not in Debug mode, no serial connection needed
#ifdef DEBUG
 #include "Serial.h"
#endif

// Time for one Bit
#define BITTIME 1000
// IR TX PIN
#define IR_TX_PIN BIT6
// Push Button Pin
#define BUTTON_PIN BIT3
// LED Pin
#define LED_PIN BIT0
// LSB (1) or MSB (0) first
#define LSBFIRST 0
// The Speed of the UARRT connection
#define SERIALSPEED 9600
// The code that is send over IR
#define CODE 240
// Smallest delay between two transmissions
#define DELAY_BETWEEN_TWO_SENDS_MS 100
// The clockSpeed in MHZ (CAUSION: needs to be changed in main as well)
#define CLK_SPEED 16
#define INIT_CLK() BCSCTL1 = CALBC1_16MHZ; DCOCTL = CALDCO_16MHZ;
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
	P1DIR |= LED_PIN;
	P1OUT &= ~LED_PIN;

	// Set Push Button to INPUT and enable PULLUPS
	P1DIR &= ~BUTTON_PIN;
	P1REN |= BUTTON_PIN;
	P1OUT |= BUTTON_PIN;

	// Set IROutput PIN to Ouput and disable
	P1DIR |= IR_TX_PIN;
	// Set output source to timer A
	P1SEL |= IR_TX_PIN;
	P1OUT &= ~IR_TX_PIN;

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
	// Loop variables
	int i = 0;
	int j = 0;
	// Latch in Byte so it can be safely modified
	int code = Byte;
	// Boolean used for parity check
	int parity = 0;

	// Send start Bit
	P1SEL |= IR_TX_PIN;
	for (j = 0; j < CLK_SPEED; j++) {
		_delay_cycles(BITTIME/2);
	}

	if (DEBUG) serialPrintInt(1);

	// If we want to send LSB First
	if (LSBFIRST) {
		// Loop over all Bits
		for (i = 0; i < 8; i++) {
			// If code and Mask not 0
			if ((code & BIT0) != 0) {
				// Send a logical 1
				P1SEL |= IR_TX_PIN;
				if (DEBUG) {
					serialPrint("\t BIT");
					serialPrintInt(i);
					serialPrint(": ");
					serialPrintInt(code & BIT0);
				}
				// Xor of the Parity Bit
				parity ^= BIT0;
			} else {
				// Send a logical 0
				if (DEBUG) {
					serialPrint("\t BIT");
					serialPrintInt(i);
					serialPrint(": ");
					serialPrintInt(code & BIT0);
				}
				P1SEL &= ~IR_TX_PIN;
				P1OUT &= ~IR_TX_PIN;
			}
			// Shift the Bit to send
			code = code >> 1;
			// Wait a Bitlength
			for (j = 0; j < CLK_SPEED; j++) {
				_delay_cycles(BITTIME);
			}
		}
	// If we want to send MSB First
	} else {
		// Loop over all Bits
		for (i = 0; i < 8; i++) {
			// If code and Mask not 0
			if ((code & BIT7) != 0) {
				// Send a logical 1
				P1SEL |= IR_TX_PIN;
				if (DEBUG) {
					serialPrint("\t BIT");
					serialPrintInt(i);
					serialPrint(": ");
					serialPrintInt((code & BIT7) >> 7);
				}
				// Xor of the Parity Bit
				parity ^= BIT0;
			} else {
				// Send a logical 0
				if (DEBUG) {
					serialPrint("\t BIT");
					serialPrintInt(i);
					serialPrint(": ");
					serialPrintInt((code & BIT7) >> 7);
				}
				P1SEL &= ~IR_TX_PIN;
				P1OUT &= ~IR_TX_PIN;
			}
			// Shift the Bit to send
			code = code << 1;
			// Wait a Bitlength
			for (j = 0; j < CLK_SPEED; j++) {
				_delay_cycles(BITTIME);
			}
		}
	}
	if (DEBUG) {
		serialPrint("\t Parity:");
		serialPrintInt(parity);
	}
	// If it was an odd number of 1s
	if (parity) {
		// send parity 1
		P1SEL |= IR_TX_PIN;
	} else {
		// else 0
		P1SEL &= ~IR_TX_PIN;
		P1OUT &= ~IR_TX_PIN;
	}
	// Send stop bit
	for (j = 0; j < CLK_SPEED; j++) {
		_delay_cycles(BITTIME);
	}
	if (DEBUG) {
		serialPrint("\t STOPBIT:");
		serialPrintInt(1);
	}
	P1SEL |= IR_TX_PIN;	// Send stop bit
	for (j = 0; j < CLK_SPEED; j++) {
		_delay_cycles(BITTIME);
	}
	// Disable PWM on line
	P1SEL &= ~IR_TX_PIN;
	P1OUT &= ~IR_TX_PIN;
	if (DEBUG) serialPrintln("");
}


int main(void) {
	// Stop whatchdog
    WDTCTL = WDTPW + WDTHOLD;

	// Set clock to 1 MHz
	INIT_CLK()
	

    if (DEBUG) serialBegin(SERIALSPEED, CLK_SPEED);

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
    	if (!(P1IN & BUTTON_PIN)) {
			// Send specifid Code
    		sendCode(CODE);
			// Display sending with LED on
			P1OUT |= LED_PIN;
			// Wait so that no Burst can be sended
			int millis = 0;
			for (millis = 0; millis < DELAY_BETWEEN_TWO_SENDS_MS; millis++) {
		   		_delay_cycles(CLK_SPEED*1000);
			}
		} else {
			// Turn LED off
			P1OUT &= ~LED_PIN;
		}
    }
}


