#include <msp430.h> 
#include "laserTagIR.h"

#define DEBUG_UART
#ifdef DEBUG_UART
  #include "laserTagUART.h"
#endif

// LED pins on Port 1
#define LED_RED = BIT3
#define LED_GREEN = BIT4
#define LED_BLUE = BIT5

volatile char irInput = 0;
volatile char irBitCount = 0;
volatile int irDataBuffer = 0;
volatile char irParityCheck = 0;

void initClocks(void) {
  // Stop Watchdog Timer
  WDTCTL = WDTPW + WDTHOLD;

  // Init MCLK to 1 MHz
  if (CALBC1_1MHZ == 0xFF || CALDCO_1MHZ == 0xFF)
    while(1);
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
}

void initIOPins(void) {
  P1SEL = 0x00;
  P1SEL2 = 0x00;
  // Set P1.x as input.
  P1DIR = 0x00 | LED_RED | LED_GREEN | LED_BLUE ;
  P1REN = 0x00;
  P1OUT = 0x00;
  // Enable interrupt on P1.6 on falling edge.
  IR_ENABLE_INTERRUPT


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
    P1DIR |= BIT0; P1OUT |= BIT0; // enable led1 in debug mode.
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

  while (1) {
    // TODO(Jan): vielleicht direkt in der timer isr machen.
    if (irBitCount == IR_NUM_BITS) {
      // Check the parity and stop bit.
      // If irParityCheck is 1, the parity bit is wrong.
      if (irDataBuffer & 0x01 && ~irParityCheck & 0x01) {
        #ifdef DEBUG_UART
          serialPrint("Success! Received: ");
        #endif
      }
    	#ifdef DEBUG_UART
    	  serialPrintInt(irDataBuffer);
    	  serialPrint("\n");
    	#endif
      irBitCount = 0;
    }
  }
}

// Port1 isr.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector=PORT1_VECTOR
  __interrupt void PORT_1 (void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(PORT1_VECTOR))) PORT_1 (void)
#else
  #error Compiler not supported!
#endif
{
  // Falling edge on P1.6 -> start bit detected.
  if (P1IFG & BIT6) {
    IR_START_TIMER
    IR_DISABLE_INTERRUPT
    irBitCount = 0;
    irDataBuffer = 0;
    irParityCheck = 0;
  }
}


// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector=TIMER0_A0_VECTOR
  __interrupt void Timer_A (void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) Timer_A (void)
#else
  #error Compiler not supported!
#endif
{
  // Read ir input (active low).
  irInput = (~P1IN & BIT6) >> 6;

  // shift ir input into the buffer and count 1s for parity check.
  irDataBuffer = (irDataBuffer << 1) | irInput;
  irBitCount++;

  // If transmission is complete, stop the timer and enable the P1.6 interrupt.
  if (irBitCount == IR_NUM_BITS) {
    // Transmission finished. Stop timer and go back to idle state.
    IR_STOP_TIMER
    IR_ENABLE_INTERRUPT
  } else if (irBitCount < IR_NUM_BITS) {
    // Data or parity bit transmitted. Update parity check. 
    irParityCheck ^= irInput;
  }
  
  TA0CCTL0 &= ~CCIFG;
}
