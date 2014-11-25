#include <msp430.h> 
#include "laserTagIR.h"

#define DEBUG_UART 1
#ifdef DEBUG_UART
  #include "laserTagUART.h"
#endif

volatile char irInput = 0;
volatile char irBitCount = 0;
volatile int irDataBuffer = 0;
volatile char irParityCheck = 0;

void initClocks(void) {
  // Stop Watchdog Timer
  WDTCTL = WDTPW + WDTHOLD;

  // Init MCLK to 16 MHz
  if (CALBC1_1MHZ ==0xFF || CALDCO_1MHZ == 0xFF)
    while(1);
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
}

void initIOPins(void) {
  P1SEL = 0x00;
  P1SEL2 = 0x00;
  // Set P1.x as input.
  P1DIR = 0x00 | BIT0;   // debugging led
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
    P1OUT = 0x00 | BIT0; // enable led1 in debug mode.
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
    // Check the parity and stop bit.
    // TODO(Jan): vielleicht direkt in der timer isr machen.
    if (irBitCount == IR_NUM_BITS) {
      if (irDataBuffer & IR_MASK_PARITY_STOP == IR_MASK_PARITY_STOP) { //TODO(Jan): tatsächlich auf parity checken!
        #ifdef DEBUG_UART
          serialPrint("Success! Received: ");
        #endif
      }
	#ifdef DEBUG_UART
  	  serialPrintInt(irDataBuffer);
  	  serialPrint("\n");
  	  irBitCount = 0;
	#endif
    }
  }
}

// Port1 isr.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector=PORT1_VECTOR
  __interrupt void PORT_1 (void) {
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(PORT1_VECTOR))) PORT_1 (void)
#else
  #error Compiler not supported!
#endif
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
  // Read ir input (active low).
  irInput = (~P1IN & BIT6) >> 6;

  // shift ir input into the buffer and count 1s for parity check.
  irDataBuffer = (irDataBuffer << 1) | irInput;
  irParityCheck += irInput;
  irBitCount++;

  // If transmission is complete, stop the timer and enable the P1.6 interrupt.
  // TODO(Jan): vielleicht interrutpt erst wieder aktivieren, wenn übertragung erfolgreich war.
  if (irBitCount == IR_NUM_BITS) {
    // Transmission finished. Stop timer and go back to idle state.
    IR_STOP_TIMER
    IR_ENABLE_INTERRUPT
  }
  
  TA0CCTL0 &= ~CCIFG;
}
