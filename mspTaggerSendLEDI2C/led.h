/*
 * led.h
 *
 *  Created on: 22.01.2015
 *      Author: Benny
 */

#ifndef LED_H_
#define LED_H_

#define NULL 0

#define CLK_SPEED 16 // The clockSpeed in MHZ (CAUTION: needs to be changed in main as well)
#define BUTTON_PIN BIT3 // Push Button Pin
#define LED_PIN BIT0 // LED Pin
#define SHOOT_PATTERN_DELAY 30 // Shoot pattern delay
#define SHOOT_PATTERN_LENGTH 7 // number of pattern elements
#define SHOOT_PATTERN_COUNT 3 // How often the shoot pattern should be shown
#define BOOT_PATTERN_DELAY 500 // Boot pattern delay
#define BOOT_PATTERN_LENGTH 7 // number of pattern elements
#define HIT_PATTERN_DELAY 150 // Hit pattern delay
#define RELOAD_PATTERN_DELAY 300 // Reload pattern delay
#define RELOAD_PATTERN_LENGTH 7 // Reload pattern length
#define AMMO_PATTERN_DELAY 300 // Reload pattern delay
#define DEAD_PATTERN_DELAY 300 // Reload pattern delay
#define HEAL_PATTERN_DELAY 300 // Health pattern delay
#define HEAL_PATTERN_LENGTH 7 // Health pattern length
#define LOW_AMMO_PATTERN_DELAY 300 // Delay between LED updates for low ammo pattern
#define LOW_AMMO_PATTERN_LENGTH 1 // Consists only of blinking
#define DEAD_PATTERN_LENGTH 1 // Consists only of blinking

// Logical Zero high duration is 500 ns ->
#define LOGICAL_ZERO_HIGH_DELAY 4//3//8
// Logical Zero high duration is 2000 ns ->
#define LOGICAL_ZERO_LOW_DELAY 23//3//16//32
// Logical One high duration is 1200 ns ->
#define LOGICAL_ONE_HIGH_DELAY 15//10//19
// Logical One low duration is 1300 ns ->
#define LOGICAL_ONE_LOW_DELAY 14//1//10//21

#define NUMB_LEDS 7

#define TIMER_CONFIG (TASSEL_1 | ID_0 | MC_1) // timer A1 control = (use ACLK | input divider = 8 | clear | enable int) | stop timer
#define TIMER_COMPARE_CONFIG (CM_0 | CCIS_3 | OUTMOD_1 | CCIE) // timer A1 capture/compare control = capturemode disabled | (unused) | output is set when done | enable interrupt
#define TIMER_COUNTS_PER_MS 12 // counter value for 1 ms

volatile int patternState = 0;
volatile int patternDelay = 0;
volatile int patternRestart = 0;
int (*patternFunction)() = NULL;

extern int health; // get the player health from the main file

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} LedColor;

LedColor ledColors[NUMB_LEDS];

LedColor BLACK = {0, 0, 0};
LedColor RED = {255, 0, 0};
LedColor GREEN = {0, 255, 0};
LedColor BLUE = {0, 0, 255};

LedColor hitColor1 = {255, 255, 255};
LedColor hitColor2 = {255, 255, 255};
LedColor hitColor3 = {255, 255, 255};

LedColor shootColor = {255, 0, 0};
LedColor bootColor = {255, 255, 255};
LedColor teamColor = {0, 255, 0};
LedColor deadColor = {255, 0, 0};
LedColor lowAmmoColor = {255, 0, 255};

typedef enum
{
	OFF,
	BOOT_PATTERN,
	HIT_PATTERN,
	SHOOT_PATTERN,
	DEAD_PATTERN,
	LOW_AMMO_PATTERN,
	HEAL_PATTERN,
	HEALTH_PATTERN,
	ERROR_PATTERN
} PATTERN_TYPE;
volatile PATTERN_TYPE curPatternType = OFF;

int changed = 1;

// sets the LED stripe pin to HIGH to transmit a logical one
inline void ledSendLogOne()
{
	P2OUT |= LED_PIN;
	__delay_cycles(LOGICAL_ONE_HIGH_DELAY);
	P2OUT &= ~LED_PIN;
	__delay_cycles(LOGICAL_ONE_LOW_DELAY);
}

// sets the LED stripe pin to HIGH to transmit a logical zero
inline void ledSendLogZero()
{
	P2OUT |= LED_PIN;
	__delay_cycles(LOGICAL_ZERO_HIGH_DELAY);
	P2OUT &= ~LED_PIN;
	__delay_cycles(LOGICAL_ZERO_LOW_DELAY);
}

// goes through the bits of the RGB value and toggles the output pin accordingly
void sendColor(uint8_t R, uint8_t G, uint8_t B) {
	int i;
	uint8_t mask = BIT0;
	for (i = 7; i >= 0; --i)
		if (G & (mask << i)) {
			ledSendLogOne();
		} else {
			ledSendLogZero();
		}

	for (i = 7; i >= 0; --i)
		if (R & (mask << i)) {
			ledSendLogOne();
		} else {
			ledSendLogZero();
		}

	for (i = 7; i >= 0; --i)
		if (B & (mask << i)) {
			ledSendLogOne();
		} else {
			ledSendLogZero();
		}

}

// apply the values in ledColors to the leds
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


int shootPattern()
{
	if (patternState >= SHOOT_PATTERN_LENGTH) // if we reached the end of the sequence, indicate that you want to quit
	{
	    sendAllLEDsOneColor(BLACK);
		return 0;
	}
	ledColors[patternState-1] = BLACK;
	ledColors[patternState] = shootColor;
	sendAllLEDs();

	return 1; // indicate that you are not done
}


int bootPattern()
{
	if (patternState > BOOT_PATTERN_LENGTH) // if we reached the end of the sequence, indicate that you want to quit
	{
		patternState = 0;
	    sendAllLEDsOneColor(BLACK);
	}
	else if (patternState > 0)
	{
		ledColors[patternState - 1] = bootColor; // state 0 is no active LED, thus (-1)
		sendAllLEDs();
	}
	return 1; // indicate that you are not done
}


int hitPattern()
{
	switch(patternState)
	{
	case 0:
		sendAllLEDsOneColor(hitColor1);
		break;
	case 1:
		sendAllLEDsOneColor(BLACK);
		break;
	case 2:
		sendAllLEDsOneColor(hitColor2);
		break;
	case 3:
		sendAllLEDsOneColor(BLACK);
		break;
	case 4:
		sendAllLEDsOneColor(hitColor3);
		break;
	default:
		sendAllLEDsOneColor(BLACK);
		return 0;
	}

	return 1; // indicate that you are not done
}


int reloadPattern()
{
	if (patternState >= RELOAD_PATTERN_LENGTH)
	{
		allLEDsOneColor(BLACK);
		return 0;
	}

	ledColors[patternState] = lowAmmoColor;
	sendAllLEDs();

	return 1;
}


int healPattern()
{
	if (patternState > HEAL_PATTERN_LENGTH - health)
	{
		patternState = 0;
	}

	sendAllLEDsOneColor(BLACK);
	int i = 0;
	for (; i < health; ++i) // show health
		ledColors[i] = teamColor;

	ledColors[health + patternState] = teamColor; // missing health is animated

	sendAllLEDs();

	return 1;
}


int lowAmmoPattern()
{
	if (patternState > LOW_AMMO_PATTERN_LENGTH)
	{
		sendAllLEDsOneColor(BLACK);
		patternState = 0;
	}
	else
		//sendAllLEDsOneColor(lowAmmoColor);
		sendAllLEDsOneColor(BLUE);

	return 1;
}


int deadPattern()
{
	if (patternState > DEAD_PATTERN_LENGTH)
	{
		sendAllLEDsOneColor(BLACK);
		patternState = 0;
	}
	else
		sendAllLEDsOneColor(deadColor);

	return 1;
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

// Pattern Timer Stuff
void startPatternTimer(int millis, int (*patternFunc)())
{
	// copy the pattern parameters to start it again if necessary
	patternDelay = millis;
	patternFunction = patternFunc;
	patternRestart = (*patternFunc)();

	// reset timer
	TA1CTL |= TACLR;
	// set compare latches value in number of clock cycles
	TA1CCR0 = TIMER_COUNTS_PER_MS * millis;
	// start timer
	TA1CCTL0 = TIMER_COMPARE_CONFIG;
	TA1CTL = TIMER_CONFIG;
}

void stopPatternTimer()
{
	TA1CTL = TACLR | MC_0;
}

void startPattern(PATTERN_TYPE type)
{
	if (type == curPatternType || curPatternType == HIT_PATTERN) // do not overwrite hit pattern
		return;
	else
	{
		switch(type)
		{
		case BOOT_PATTERN:
			startPatternTimer(BOOT_PATTERN_DELAY, bootPattern);
			break;
		case HIT_PATTERN:
			startPatternTimer(HIT_PATTERN_DELAY, hitPattern);
			break;
		case SHOOT_PATTERN:
			startPatternTimer(SHOOT_PATTERN_DELAY, shootPattern);
			break;
		case LOW_AMMO_PATTERN:
			startPatternTimer(LOW_AMMO_PATTERN_DELAY, lowAmmoPattern);
			break;
		case DEAD_PATTERN:
			startPatternTimer(DEAD_PATTERN_DELAY, deadPattern);
			break;
		case HEALTH_PATTERN:
			showCurrentHealth(health);
			break;
		case ERROR_PATTERN:
		default:
			showErrorPattern();
		}
		curPatternType = type;
	}

}


// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector=TIMER1_A0_VECTOR
  __interrupt void Timer_A (void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A (void)
#else
  #error Compiler not supported!
#endif
{
	  if (patternRestart) // the pattern function is not done
	  {
		  patternState++;
		  startPatternTimer(patternDelay, patternFunction); // so start it again
	  }
	  else // pattern done, reset state and cancel timer
	  {
		  patternState = 0;
		  sendAllLEDsOneColor(BLACK);
		  stopPatternTimer();
		  curPatternType = OFF;
	  }

	  // remove interrupt flag
	  TA1CCTL0 &= ~CCIFG;
}

#endif /* LED_H_ */
