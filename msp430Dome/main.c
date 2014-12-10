#include <msp430.h> 
#include "laserTagIR.h"
#include "i2c_slave.h"

//#define DEBUG_UART
#ifdef DEBUG_UART
  #include "laserTagUART.h"
#endif

// LED pins on Port 1
#define LED_RED BIT3
#define LED_GREEN BIT4
#define LED_BLUE BIT5

// Variables for ir receiver.
volatile unsigned char irInput = 0;
volatile unsigned char irBitCount = 0;
volatile unsigned int irDataBuffer = 0;
volatile unsigned char irParityCheck = 0;
volatile unsigned char irValid = 0;

void initClocks (void) {
  // Stop Watchdog Timer
  WDTCTL = WDTPW + WDTHOLD;

  // Init MCLK to 16 MHz
  if (CALBC1_16MHZ == 0xFF || CALDCO_16MHZ == 0xFF)
    while(1);
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL  = CALDCO_16MHZ;

  // Devide SMCLK by 2 (DIVS_1) for 8MHz clock (timer devides by 8 to count microseconds).
  BCSCTL2 = DIVS_1;
}

void initIOPins (void) {
  P1SEL = 0x00;
  P1SEL2 = 0x00;
  // Set P1.x as input.
  P1DIR = 0x00 | LED_RED | LED_GREEN | LED_BLUE;
  P1REN = 0x00;
  P1OUT = 0x00;
  // Enable interrupt on IR_RECEIVER_PIN on falling edge.
  IR_ENABLE_INTERRUPT


  #ifdef DEBUG_UART
    // Activate UART on 1.1 / 1.2
    // (fixed connection to PC, shows up there as COMx)
    P1SEL = BIT1 + BIT2;                      // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = (69 & 0xFF);                   // 8MHz 9600
    UCA0BR1 = (69 >> 8);                     // 8MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
  #endif
}

void flashHitLed (void) {
  P1OUT |= LED_RED;
  __delay_cycles(1600000);
  P1OUT &= ~LED_RED;
  P1OUT |= LED_GREEN;
  __delay_cycles(1600000);
  P1OUT &= ~LED_GREEN;
  P1OUT |= LED_BLUE;
  __delay_cycles(1600000);
  P1OUT &= ~LED_BLUE;
}

// I2C callback functions.
volatile unsigned char i2cBuffer; // TODO durch ringbuffer ersetzen.

void start_cb() {}

void transmit_cb(unsigned char volatile *value)
{
  *value = i2cBuffer;
  i2cBuffer = 0;
}

void receive_cb(unsigned char value) {}

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

  slave_i2c_init(start_cb, transmit_cb, receive_cb, 0x68);
  flashHitLed();

  while (1) {
    // TODO(Jan): vielleicht direkt in der timer isr machen.
    if (irValid) {
      // Check the parity and stop bit.
      // If irParityCheck is 1, the parity bit is wrong.
      if (irDataBuffer & 0x01 && ~irParityCheck & 0x01) {
        #ifdef DEBUG_UART
          serialPrint("Success! Received: ");
        #endif
        i2cBuffer = irDataBuffer >> 2;
        flashHitLed();
      }
      #ifdef DEBUG_UART
        serialPrintInt(irDataBuffer);
        serialPrint("\n");
      #endif
      irValid = 0;
      IR_ENABLE_INTERRUPT
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
  // Falling edge on IR_RECEIVER_PIN -> start bit detected.
  if (P1IFG & IR_RECEIVER_PIN) {
    IR_START_TIMER
    IR_DISABLE_INTERRUPT
    irBitCount = 0;
    irDataBuffer = 0;
    irParityCheck = 0;
  }
  // wait for 1 ms and check again if start bit was long enough.
  __delay_cycles(16000);
  if (P1IN & IR_RECEIVER_PIN) {
    // If pin is 1 start bit was too short.
    IR_STOP_TIMER
    IR_ENABLE_INTERRUPT
  }
}


// Timer A1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector=TIMER1_A0_VECTOR
  __interrupt void Timer_A (void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(TIMER1_A0_VECTOR))) Timer_A (void)
#else
  #error Compiler not supported!
#endif
{
  // Read ir input (active low).
  irInput = (~P1IN & IR_RECEIVER_PIN);

  // shift ir input into the buffer and count 1s for parity check.
  irDataBuffer = (irDataBuffer << 1) | irInput;
  irBitCount++;

  // If transmission is complete, stop the timer and enable the P1.6 interrupt.
  if (irBitCount == IR_NUM_BITS) {
    // Transmission finished. Stop timer and go back to idle state.
    IR_STOP_TIMER
    irValid = 1;
  } else if (irBitCount < IR_NUM_BITS) {
    // Data or parity bit transmitted. Update parity check. 
    irParityCheck ^= irInput;
  }
  
  TA1CCTL0 &= ~CCIFG;
}
