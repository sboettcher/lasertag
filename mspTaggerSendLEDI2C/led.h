/*
 * led.h
 *
 *  Created on: 22.01.2015
 *      Author: Benny
 */

#ifndef LED_H_
#define LED_H_

// The clockSpeed in MHZ (CAUSION: needs to be changed in main as well)
#define CLK_SPEED 16
// Push Button Pin
#define BUTTON_PIN BIT3
// LED Pin
#define LED_PIN BIT0
// Shoot pattern delay
#define SHOOT_PATTERN_DELAY 30
// How often the shoot pattern should be shown
#define SHOOT_PATTERN_COUNT 3
// Boot pattern delay
#define BOOT_PATTERN_DELAY 500
// Hit pattern delay
#define HIT_PATTERN_DELAY 150
// Reload pattern delay
#define RELOAD_PATTERN_DELAY 300
// Reload pattern delay
#define AMMO_PATTERN_DELAY 300
// Reload pattern delay
#define DEAD_PATTERN_DELAY 300
// Reload pattern delay
#define HEALTH_PATTERN_DELAY 300

// Logical Zero high duration is 500 ns ->
#define LOGICAL_ZERO_HIGH_DELAY 4//3//8
// Logical Zero high duration is 2000 ns ->
#define LOGICAL_ZERO_LOW_DELAY 23//3//16//32
// Logical One high duration is 1200 ns ->
#define LOGICAL_ONE_HIGH_DELAY 15//10//19
// Logical One low duration is 1300 ns ->
#define LOGICAL_ONE_LOW_DELAY 14//1//10//21

#define NUMB_LEDS 7

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

struct ledcolor hitColor1 = {255, 255, 255};
struct ledcolor hitColor2 = {255, 255, 255};
struct ledcolor hitColor3 = {255, 255, 255};

struct ledcolor shootColor = {255, 0, 0};

struct ledcolor bootColor = {255, 255, 255};

struct ledcolor teamColor = {255, 0, 255};

struct ledcolor deadColor = {255, 0, 0};

struct ledcolor lowAmmoColor = {255, 255, 0};

int changed = 1;

void sendColor(uint8_t R, uint8_t G, uint8_t B) {
    //_________________________GREEN____________________________
	if ((G & BIT7) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT6) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT5) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT4) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT3) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT2) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT1) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((G & BIT0) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}

    //_________________________RED____________________________
	if ((R & BIT7) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT6) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT5) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT4) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT3) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT2) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT1) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((R & BIT0) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}


    //_________________________BLUE____________________________
	if ((B & BIT7) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT6) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT5) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT4) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT3) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT2) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT1) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}
	if ((B & BIT0) != 0) {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ONE_LOW_DELAY);
	} else {
		P2OUT |= LED_PIN;
		__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
		P2OUT &= ~LED_PIN;
		__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
	}

}


void sendAllLEDs() {
	int i = 0;
	for (i = 0; i < NUMB_LEDS; i++) {
		sendColor(ledColors[i].red, ledColors[i].green, ledColors[i].blue);
	}
	changed = 1;
}

void allLEDsOneColor(LedColor color) {
	int i = 0;
	for (i = 0; i < NUMB_LEDS; i++) {
		ledColors[i] = color;
	}
}

void allLEDsOneColorRGB(uint8_t R, uint8_t G, uint8_t B) {
	LedColor color = {R, G, B};
	allLEDsOneColor(color);
}

void sendAllLEDsOneColor(LedColor color) {
	allLEDsOneColor(color);
	sendAllLEDs();
}

void sendAllLEDsOneColorRGB(uint8_t R, uint8_t G ,uint8_t B) {
	allLEDsOneColorRGB(R,G,B);
	sendAllLEDs();
}


void sendOneLEDColor(int led, LedColor color) {
	ledColors[led] = color;
	sendAllLEDs();
}

void sendOneLEDRGB(int led, uint8_t R, uint8_t G, uint8_t B) {
	LedColor color = {R, G, B};
	ledColors[led] = color;
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

		sendAllLEDsOneColorRGB(red, green, blue);
		for (millis = 0; millis < fadeTime; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
}

void showShootPattern() {
	int millis = 0;
	int state = 0;

	sendAllLEDsOneColor(BLACK);
	for (state = 0; state < 7; state++) {
		if (state != 0) {
			ledColors[state-1] = BLACK;
		}
		ledColors[state] = shootColor;
		sendAllLEDs();
		for (millis = 0; millis < SHOOT_PATTERN_DELAY; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
    sendAllLEDsOneColor(BLACK);
}


void showBootPattern() {
	int millis = 0;
	int state = 0;

	for (state = 0; state < 7; state++) {
		ledColors[state] = bootColor;
		sendAllLEDs();
		for (millis = 0; millis < BOOT_PATTERN_DELAY; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
    sendAllLEDsOneColor(BLACK);
}


void showHitPattern() {
	int millis = 0;

    sendAllLEDsOneColor(hitColor1);
	for (millis = 0; millis < HIT_PATTERN_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    sendAllLEDsOneColor(hitColor2);
	for (millis = 0; millis < HIT_PATTERN_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    sendAllLEDsOneColor(hitColor3);
	for (millis = 0; millis < HIT_PATTERN_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    sendAllLEDsOneColor(BLACK);
}

void showReloadPattern() {
	int millis = 0;
	int state = 0;

    allLEDsOneColor(BLACK);
	for (state = 0; state < 7; state++) {
		ledColors[state] = lowAmmoColor;
		sendAllLEDs();
		for (millis = 0; millis < RELOAD_PATTERN_DELAY; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
    sendAllLEDsOneColor(BLACK);

}

void showHealthPattern(int health) {
	int millis = 0;
	int state = 0;

	for (state = 0; state < health; state++) {
		ledColors[state] = teamColor;
	}
	for (state = health; state < 7; state++) {
		ledColors[state] = teamColor;
		sendAllLEDs();
		for (millis = 0; millis < HEALTH_PATTERN_DELAY; millis++) {
			_delay_cycles(CLK_SPEED*1000);
		}
	}
    sendAllLEDsOneColor(BLACK);
}

void showLowAmmoPattern() {
	int millis = 0;
	sendAllLEDsOneColor(lowAmmoColor);
	for (millis = 0; millis < AMMO_PATTERN_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    sendAllLEDsOneColor(BLACK);
	for (millis = 0; millis < AMMO_PATTERN_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}

}

void showDeadPattern() {
	int millis = 0;
	sendAllLEDsOneColor(deadColor);
	for (millis = 0; millis < DEAD_PATTERN_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
    sendAllLEDsOneColor(BLACK);
	for (millis = 0; millis < DEAD_PATTERN_DELAY; millis++) {
		_delay_cycles(CLK_SPEED*1000);
	}
}


void showCurrentHealth(int health) {
	if (changed) {
		int state = 0;

		for (state = 0; state < health; state++) {
			ledColors[state] = teamColor;
		}
		for (state = health; state < 7; state++) {
			ledColors[state] = BLACK;
		}
		sendAllLEDs();
		changed = 0;
	}
}

void showErrorPattern() {
	fade(BLACK, RED, 1);
	fade(RED, GREEN, 1);
	fade(GREEN, BLUE, 1);
	fade(BLUE, BLACK, 1);
}


void setTeamColor(int R, int G, int B) {
	teamColor.red = R;
	teamColor.green = G;
	teamColor.blue = B;
}


#endif /* LED_H_ */
