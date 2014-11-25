#include <msp430.h>
#include "laserTagIR.h"

#define DEBUG_UART 1
#ifdef DEBUG_UART
  #include "laserTagUART.h"
#endif

volatile char irState = STATE_IDLE;
volatile char irCurrentBit = 0;
volatile char irInput = 0;
volatile char irDataBuffer = 0;
volatile char irParityCheck = 0;

void initClocks(void) {
  // Stop Watchdog Timer
  WDTCTL = WDTPW + WDTHOLD;

  // Init MCLK to 1 MHz
  if (CALBC1_16MHZ ==0xFF || CALDCO_16MHZ == 0xFF)
    while(1);
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL  = CALDCO_16MHZ;
  BCSCTL2 |= DIVS_1;
}

void initIOPins(void) {
  P1SEL = 0x00;
  P1SEL2 = 0x00;
  // Set P1.x as input.
  P1DIR = 0x00 | BIT0 | BIT4;   // debugging led
  P1REN = 0x00;
  P1OUT = 0x00;
  // Enable interrupt on P1.6 on falling edge.
  P1IE = 0x00 | BIT6;
  P1IES = 0x00 | BIT6;
  P1IFG = 0x00;


  #ifdef DEBUG_UART
    // Activate UART on 1.1 / 1.2
    // (fixed connection to PC, shows up there as COMx)
    P1SEL = BIT1 + BIT2;                      // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
    P1OUT = 0x00 | BIT0;
  #endif
}

/*
 * main.c
 */
int main(void) {
  initClocks();
  initIOPins();
  __enable_interrupt();

  #ifdef DEBUG_UART
    serialPrint("MSP430 booted!\n");
  #endif

  while(1);
}

// Port1 isr.
#pragma vector=PORT1_VECTOR
__interrupt void PORT_1 (void) {
  // Falling edge on P1.6 -> start bit detected.
  if (P1IFG & BIT6 && irState == STATE_IDLE) {
    IR_START_TIMER;
    irState = STATE_READ_DATA;
    irCurrentBit = 0;
    irDataBuffer = 0;
    irParityCheck = 0;
    P1OUT |= BIT4;
    //TODO(Jan): vielleicht besser den interupt auf dem pin komplett zu deaktivieren.
  }
  P1IFG = 0x00;
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void) {

  P1OUT ^= BIT4;
  irCurrentBit++;
  if (irCurrentBit == IR_NUM_BITS) {
    IR_STOP_TIMER
    irState = STATE_IDLE;
    P1OUT &= ~BIT4;
  }

  TA0CCTL0 &= ~CCIFG;
}
