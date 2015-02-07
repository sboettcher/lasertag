#include <msp430.h> 
#include <stdint.h>
#include "serial.h"
#include "led.h"
#include "i2c_slave.h"

#define DEBUG 0

// The adress under which this device should responde
#define I2C_ADRESS 0x61

#define COMMAND_LOW_AMMO 'a'
#define COMMAND_RELOAD 'r'
#define COMMAND_DEAD 'd'
#define COMMAND_HEALTH 'h'
#define COMMAND_HEALTH_1 '1'
#define COMMAND_HEALTH_2 '2'
#define COMMAND_HEALTH_3 '3'
#define COMMAND_HEALTH_4 '4'
#define COMMAND_HEALTH_5 '5'
#define COMMAND_HEALTH_6 '6'
#define COMMAND_HEALTH_7 '7'
#define COMMAND_DEACTIVATE 'x'
#define COMMAND_ACTIVATE 'y'
#define COMMAND_SHOOT 's'
#define COMMAND_BOOT 'b'
#define COMMAND_READY 'e'
#define COMMAND_CODE 'c'
#define COMMAND_TEAM_COLOR 't'


#define BITTIME 1000
// IR TX PIN
#define IR_TX_PIN BIT6
// Push Button Pin
#define BUTTON_PIN BIT3
// LSB (1) or MSB (0) first
#define LSBFIRST 0
// The Speed of the UARRT connection
#define SERIALSPEED 9600
// Smallest delay between two transmissions
#define DELAY_BETWEEN_TWO_SENDS_MS 100
// The clockSpeed in MHZ (CAUSION: needs to be changed in main as well)
#define CLK_SPEED 16
#define INIT_CLK() BCSCTL1 = CALBC1_16MHZ; DCOCTL = CALDCO_16MHZ;
// The PWM frequency in kHz
#define PWM_FREQUZENCY 38
// The PWM periode
#define PWM_PERIODE (CLK_SPEED*1000.0/PWM_FREQUZENCY)

#define START_BYTE 0x7e

volatile unsigned char i2cBuffer; // TODO durch ringbuffer ersetzen.

int booted = 0; //TODO: 0;
int health = NUMB_LEDS;
int canShoot = 1;
int isDead = 0;
int isActive = 1;
int getCode = 0;
int getColor = 0;
// The code that is send over IR
int CODE = 240;
int transmissionCheck = 0;

/*
 * Init the Ports
 */
void initPorts() {
	// Set LED1 to OUTPUT and to 0
	P2DIR |= LED_PIN;
	P2OUT &= ~LED_PIN;

	// Set Push Button to INPUT and enable PULLUPS
	P1DIR &= ~BUTTON_PIN;
	P1REN |= BUTTON_PIN;
	P1OUT |= BUTTON_PIN;


	// Set IROutput PIN to Ouput and disable
	P2DIR |= IR_TX_PIN;
	// Reset XOUT
	P2SEL &= ~BIT7;
	// Set output source to timer A
	P2SEL |= IR_TX_PIN;

	// PWM setup
	// Set the PWM periode to 38kHz
	TA0CCR0 = PWM_PERIODE;
	// Set dutyCycles to 50%
    TA0CCR1 = PWM_PERIODE/2;
    // Set output mode to resete/set
	TA0CCTL1 |= OUTMOD_7;
	// Set the counter to "count up" to TA0CCR0 with clock input "SMCLK"
	TA0CTL |= TASSEL_2 + MC_1;

	// Disable Output for the moment
	// Set output source to timer A
	P2SEL &= ~IR_TX_PIN;
	P2OUT &= ~IR_TX_PIN;

	__enable_interrupt();
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
	P2SEL |= IR_TX_PIN;
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
				P2SEL |= IR_TX_PIN;
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
				P2SEL &= ~IR_TX_PIN;
				P2OUT &= ~IR_TX_PIN;
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
				P2SEL |= IR_TX_PIN;
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
				P2SEL &= ~IR_TX_PIN;
				P2OUT &= ~IR_TX_PIN;
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
		P2SEL |= IR_TX_PIN;
	} else {
		// else 0
		P2SEL &= ~IR_TX_PIN;
		P2OUT &= ~IR_TX_PIN;
	}
	// Send stop bit
	for (j = 0; j < CLK_SPEED; j++) {
		_delay_cycles(BITTIME);
	}
	if (DEBUG) {
		serialPrint("\t STOPBIT:");
		serialPrintInt(1);
	}
	P2SEL |= IR_TX_PIN;	// Send stop bit
	for (j = 0; j < CLK_SPEED; j++) {
		_delay_cycles(BITTIME);
	}
	// Disable PWM on line
	P2SEL &= ~IR_TX_PIN;
	P2OUT &= ~IR_TX_PIN;
	if (DEBUG) serialPrintln("");
}


void shoot() {
	// Needed loop variables
	int i = 0;
	// Send specifid Code
	sendCode(CODE);
	i2cBuffer = 's';
	// Display sending with LED on
	for (i = 0; i < SHOOT_PATTERN_COUNT; i++) showShootPattern();
	// Wait so that no Burst can be sended
	int millis = 0;
	for (millis = 0; millis < DELAY_BETWEEN_TWO_SENDS_MS; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
}


// I2C callback functions.

void start_cb() {}

void transmit_cb(unsigned char volatile *value) {
  *value = i2cBuffer;
  i2cBuffer = 0;
}

void receive_cb(unsigned char value) {
	changed = 1;
	if (getCode == 1) {
		CODE = value;
		getCode = 0;
		transmissionCheck = 0;
	} else if (getColor == 1) {
		teamColor.red = value;
		getColor++;
	} else if (getColor == 2) {
		teamColor.green = value;
		getColor++;
	} else if (getColor == 3) {
		teamColor.blue = value;
		showCurrentHealth(health);
		getColor = 0;
		transmissionCheck = 0;
	} else {
		if (!transmissionCheck) {
			if (value == START_BYTE) {
				transmissionCheck = 1;
			}
		} else {
			switch (value) {
				// Low Ammo case
				case COMMAND_LOW_AMMO: {
					canShoot = 0;
					break;
				}
				case COMMAND_RELOAD: {
					canShoot = 1;
					break;
				}
				case COMMAND_DEAD: {
					isDead = 1;
					break;
				}
				case COMMAND_DEACTIVATE: {
					isActive = 0;
					break;
				}
				case COMMAND_ACTIVATE: {
					isActive = 1;
					break;
				}
				case COMMAND_HEALTH: {
					health = NUMB_LEDS;
					isDead = 0;
					showHealthPattern(health);
					break;
				}
				case COMMAND_HEALTH_1: {
					health = 1;
					showCurrentHealth(health);
					break;
				}
				case COMMAND_HEALTH_2: {
					health = 2;
					showCurrentHealth(health);
					break;
				}
				case COMMAND_HEALTH_3: {
					health = 3;
					showCurrentHealth(health);
					break;
				}
				case COMMAND_HEALTH_4: {
					health = 4;
					showCurrentHealth(health);
					break;
				}
				case COMMAND_HEALTH_5: {
					health = 5;
					showCurrentHealth(health);
					break;
				}
				case COMMAND_HEALTH_6: {
					health = 6;
					showCurrentHealth(health);
					break;
				}
				case COMMAND_HEALTH_7: {
					health = 7;
					showCurrentHealth(health);
					break;
				}
				case COMMAND_SHOOT: {
					shoot();
					break;
				}
				case COMMAND_BOOT: {
					booted = 0;
					break;
				}
				case COMMAND_READY: {
					booted = 1;
					break;
				}
				case COMMAND_CODE: {
					getCode = 1;
					break;
				}
				case COMMAND_TEAM_COLOR: {
					getColor = 1;
					break;
				}
			}
			transmissionCheck = 0;
		}
	}

}

void initI2C (void) {
  // I2C adress is determined by the 3 jumpers on P2.0 P2.1 P2.3.
  char domeId = P2IN & 0x7;
  slave_i2c_init(start_cb, transmit_cb, receive_cb, (I2C_ADRESS));
}

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	// Set clock to 16 MHz
	INIT_CLK()

	// Init the Ports and the timers
    initPorts();

	// Init the I2C lines
    initI2C();

    // Init LEDs to black
    sendAllLEDsOneColor(BLACK);

    // Show the boot up pattern at least once
    showBootPattern();
    while (!booted)	showBootPattern();



	// Do forever and for always
    while(1) {
    	if (!booted) {
    		showErrorPattern();
    	} else if (isDead) {
    		showDeadPattern();
    	} else if (!canShoot) {
    		showLowAmmoPattern();
    	} else {
    		showCurrentHealth(health);
    		// If Push Button pressed
        	while (!(P1IN & BUTTON_PIN)) {
        		shoot();
    		}
    	}
    }
	return 0;
}
