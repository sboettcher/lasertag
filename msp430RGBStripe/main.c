#include <msp430.h> 
#include "Serial.h"
#include <stdint.h>

#define DEBUG 0
// Push Button Pin
#define BUTTON_PIN BIT3
// LED Pin
#define LED_PIN BIT0
// The Speed of the UARRT connection
#define SERIALSPEED 9600
// Shoot pattern delay
#define SHOOT_PATTER_DELAY 30
// Boot pattern delay
#define BOOT_PATTER_DELAY 500
// Hit pattern delay
#define HIT_PATTER_DELAY 150
// Reload pattern delay
#define RELOAD_PATTER_DELAY 300
// Reload pattern delay
#define AMMO_PATTER_DELAY 300
// Reload pattern delay
#define DEAD_PATTER_DELAY 300
// Reload pattern delay
#define HEALTH_PATTER_DELAY 300
// The clockSpeed in MHZ (CAUSION: needs to be changed in main as well)
#define CLK_SPEED 16
#define INIT_CLK() BCSCTL1 = CALBC1_16MHZ; DCOCTL = CALDCO_16MHZ;

// Logical Zero high duration is 500 ns ->
#define LOGICAL_ZERO_HIGH_DELAY 4//3//8
// Logical Zero high duration is 2000 ns ->
#define LOGICAL_ZERO_LOW_DELAY 23//3//16//32
// Logical One high duration is 1200 ns ->
#define LOGICAL_ONE_HIGH_DELAY 15//10//19
// Logical One low duration is 1300 ns ->
#define LOGICAL_ONE_LOW_DELAY 14//1//10//21

#define NUMB_LEDS 14

struct ledcolor{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

typedef struct ledcolor LedColor ;

struct ledcolor ledColors[NUMB_LEDS];

struct ledcolor BLACK = {0, 0, 0};
struct ledcolor RED = {255, 0, 0};
struct ledcolor GREEN = {0, 255, 0};
struct ledcolor BLUE = {0, 0, 255};

struct ledcolor hitColor1 = {255, 0, 0};
struct ledcolor hitColor2 = {0, 255, 0};
struct ledcolor hitColor3 = {0, 0, 255};

struct ledcolor shootColor = {255, 0, 0};

struct ledcolor bootColor = {255, 255, 255};

struct ledcolor teamColor = {255, 0, 255};

struct ledcolor deadColor = {255, 0, 0};

struct ledcolor lowAmmoColor = {255, 255, 0};


int health = 7;
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
}




void sendColor(uint8_t R, uint8_t G, uint8_t B) {

	if ((G & BIT7) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT6) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT5) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT4) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT3) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT2) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT1) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT0) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}



	if ((R & BIT7) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT6) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT5) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT4) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT3) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT2) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT1) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT0) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}





	if ((B & BIT7) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT6) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT5) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT4) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT3) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT2) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT1) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT0) != 0) {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P1OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P1OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}

}



void sendAllLEDs() {
	int i = 0;
	for (i = 0; i < NUMB_LEDS; i++) {
		sendColor(ledColors[i].red, ledColors[i].green, ledColors[i].blue);
	}
}

void allLEDsOneColorRGB(uint8_t R, uint8_t G, uint8_t B) {
	int i = 0;
	for (i = 0; i < NUMB_LEDS; i++) {
		ledColors[i].blue = B;
		ledColors[i].red = R;
		ledColors[i].green = G;
	}
}

void allLEDsOneColor(LedColor color) {
	int i = 0;
	for (i = 0; i < NUMB_LEDS; i++) {
		ledColors[i].blue = color.blue;
		ledColors[i].red = color.red;
		ledColors[i].green = color.green;
	}
}

void shootPattern() {
	int millis = 0;
	int state = 0;

	for (state = 0; state < 7; state++) {
		if (state != 0) {
			ledColors[state-1] = BLACK;
		}
		ledColors[state] = shootColor;
		ledColors[state+6] = BLACK;
		ledColors[state+7] = shootColor;
		sendAllLEDs();
		for (millis = 0; millis < SHOOT_PATTER_DELAY; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
    allLEDsOneColor(BLACK);
	sendAllLEDs();
}


void bootPattern() {
	int millis = 0;
	int state = 0;

	for (state = 0; state < 7; state++) {
		ledColors[state] = bootColor;
		ledColors[state+7] = bootColor;
		sendAllLEDs();
		for (millis = 0; millis < BOOT_PATTER_DELAY; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
    allLEDsOneColor(BLACK);
	sendAllLEDs();
}


void hitPattern() {
	int millis = 0;

    allLEDsOneColor(hitColor1);
	sendAllLEDs();
	for (millis = 0; millis < HIT_PATTER_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    allLEDsOneColor(hitColor2);
	sendAllLEDs();
	for (millis = 0; millis < HIT_PATTER_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    allLEDsOneColor(hitColor3);
	sendAllLEDs();
	for (millis = 0; millis < HIT_PATTER_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    allLEDsOneColor(BLACK);
	sendAllLEDs();
}

void reloadPattern() {
	int millis = 0;
	int state = 0;

    allLEDsOneColor(BLACK);
	for (state = 0; state < 7; state++) {
		ledColors[state] = lowAmmoColor;
		ledColors[state+7] = lowAmmoColor;
		sendAllLEDs();
		for (millis = 0; millis < RELOAD_PATTER_DELAY; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
    allLEDsOneColor(BLACK);
	sendAllLEDs();

}

void healthPattern() {
	int millis = 0;
	int state = 0;

	for (state = 0; state < health; state++) {
		ledColors[state] = teamColor;
		ledColors[state+7] = teamColor;
	}
	for (state = health; state < 7; state++) {
		ledColors[state] = teamColor;
		ledColors[state+7] = teamColor;
		sendAllLEDs();
		for (millis = 0; millis < HEALTH_PATTER_DELAY; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
    allLEDsOneColor(BLACK);
	sendAllLEDs();
}

void lowAmmoPattern() {
	int millis = 0;
	allLEDsOneColor(lowAmmoColor);
	sendAllLEDs();
	for (millis = 0; millis < AMMO_PATTER_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    allLEDsOneColor(BLACK);
	sendAllLEDs();
	for (millis = 0; millis < AMMO_PATTER_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}

}

void deadPattern() {
	int millis = 0;
	allLEDsOneColor(deadColor);
	sendAllLEDs();
	for (millis = 0; millis < DEAD_PATTER_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    allLEDsOneColor(BLACK);
	sendAllLEDs();
	for (millis = 0; millis < DEAD_PATTER_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
}


void showCurrentHealth() {
	int state = 0;

	for (state = 0; state < health; state++) {
		ledColors[state] = teamColor;
		ledColors[state+7] = teamColor;
	}
	sendAllLEDs();
}


void fade(LedColor startColor, LedColor goalColor, int fadeTime) {
	int millis = 0;
	uint8_t red = startColor.red;
	uint8_t blue = startColor.blue;
	uint8_t green = startColor.green;
	uint8_t changedSth = 0;
	while(1) {
		changedSth = 0;
		if (red < goalColor.red) {
			red = red + 1;
			changedSth = changedSth + 1;
		} else if (red > goalColor.red) {
			red = red - 1;
			changedSth = changedSth + 1;
		}
		if (green < goalColor.green) {
			green = green + 1;
			changedSth = changedSth + 1;
		} else if (green > goalColor.green) {
			green = green - 1;
			changedSth = changedSth + 1;
		}
		if (blue < goalColor.blue) {
			blue = blue + 1;
			changedSth = changedSth + 1;
		} else if (blue > goalColor.blue) {
			blue = blue - 1;
			changedSth = changedSth + 1;
		}
		if (changedSth == 0) break;

		allLEDsOneColorRGB(red, green, blue);
		sendAllLEDs();
		for (millis = 0; millis < fadeTime; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
}

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	// Set clock to 16 MHz
	INIT_CLK()

	int millis = 0;

	// Init the Ports and the timers
    initPorts();

    // Init LEDs to black
    allLEDsOneColor(BLACK);
	sendAllLEDs();

	fade(BLACK, RED, 10);
	fade(RED, BLUE, 10);
	fade(BLUE, GREEN, 10);
	fade(GREEN, BLACK, 10);

	bootPattern();
	for (millis = 0; millis < 500; millis++) {
			_delay_cycles(CLK_SPEED*1000);
	}

	showCurrentHealth();
	sendAllLEDs();

	// Do forever and for always
    while(1) {
		// If Push Button pressed
    	if (!(P1IN & BUTTON_PIN)) {
		    allLEDsOneColor(BLACK);
			sendAllLEDs();

			health = 7;

			int i = 0;
			int timeBetweenPatterns = 1000;

			for (i = 0; i < 5; i++) shootPattern();
			showCurrentHealth();
			for (millis = 0; millis < timeBetweenPatterns; millis++) {
				_delay_cycles(CLK_SPEED*1000);
			}

			for (i = 0; i < 4; i++) {
				hitPattern();
				health = health - 1;
				showCurrentHealth();
				for (millis = 0; millis < timeBetweenPatterns; millis++) {
					_delay_cycles(CLK_SPEED*1000);
				}
			}


			showCurrentHealth();
			for (millis = 0; millis < timeBetweenPatterns; millis++) {
				_delay_cycles(CLK_SPEED*1000);
			}


			for (i = 0; i < 4; i++) lowAmmoPattern();
			showCurrentHealth();
			for (millis = 0; millis < timeBetweenPatterns; millis++) {
				_delay_cycles(CLK_SPEED*1000);
			}

			reloadPattern();
			showCurrentHealth();
			for (millis = 0; millis < timeBetweenPatterns; millis++) {
				_delay_cycles(CLK_SPEED*1000);
			}


			for (i = 0; i < 3; i++) {
				for (millis = 0; millis < timeBetweenPatterns; millis++) {
					_delay_cycles(CLK_SPEED*1000);
				}
				hitPattern();
				health = health - 1;
				showCurrentHealth();
			}

			for (i = 0; i < 5; i++) deadPattern();
			showCurrentHealth();
			for (millis = 0; millis < timeBetweenPatterns; millis++) {
				_delay_cycles(CLK_SPEED*1000);
			}

			healthPattern();
			health = 7;
			showCurrentHealth();
			for (millis = 0; millis < timeBetweenPatterns; millis++) {
				_delay_cycles(CLK_SPEED*1000);
			}


			hitPattern();
			health = health - 1;
			showCurrentHealth();

			sendAllLEDs();
		} else {
			// Turn LED off
			P1OUT &= ~LED_PIN;
		}
    }
	return 0;
}
