#include <msp430.h> 
#include "laserTagIR.h"
#include "i2c_slave.h"

#define DEBUG_UART
#ifdef DEBUG_UART
  #include "laserTagUART.h"
#endif

/**
 * Variables and constant defines
 */

#define IS_TAGGER_RECEIVER
#define BUZZER BIT3

// I2C
#define I2C_BASE_ADRESS 0x60
#define SERIAL_START_BYTE (char)0xFF
#define SERIAL_STOP_BYTE (char)0xFE
volatile unsigned char i2cTxBuffer;
volatile unsigned char i2cRxBuffer[2]; // Buffers [teamColor, ownTaggerCode]
volatile unsigned char i2cRxCursor = 0;

volatile unsigned char outOfEnergy = 0;

// RGB LED on Port 1
#define LED_RED BIT3
#define LED_GREEN BIT4
#define LED_BLUE BIT5
#define COLOR_WHITE 0x38
#define COLOR_BLACK 0
#define FLASH_CYCLES 2400000
volatile unsigned char teamColor = COLOR_WHITE; // color is encoded by 0bBGR
volatile unsigned char ownTaggerCode = 0;
volatile char i;

// IR receiver.
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
  P2SEL = 0x00;
  P2SEL2 = 0x00;
  // Enable pull-up on P2.0 P2.1 and P2.2 or buzzer out on P2.3.
  #ifdef IS_TAGGER_RECEIVER
    P1DIR = 0x00;
    P2DIR = BUZZER;
  #elif
    P1DIR = 0x00 | LED_RED | LED_GREEN | LED_BLUE;
    P2DIR = 0x00;
    P2REN = BIT0 | BIT1 | BIT2;
    P2OUT = BIT0 | BIT1 | BIT2;
  #endif
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
    UCA0BR0 = (833 & 0xFF);                   // 8MHz 9600
    UCA0BR1 = (833 >> 8);                     // 8MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
  #endif
}

void flashLed (unsigned char color1, unsigned char color2, char repetitions) {
  for (i = 0; i < repetitions; i++) {
    #ifdef IS_TAGGER_RECEIVER
      P2OUT |= BUZZER;
      __delay_cycles(FLASH_CYCLES);
      P2OUT &= ~BUZZER;
      __delay_cycles(FLASH_CYCLES);
    #elif
      P1OUT &= ~COLOR_WHITE; // first clear color and then set new color.
      P1OUT |= color1;
      __delay_cycles(FLASH_CYCLES);
      P1OUT &= ~COLOR_WHITE;
      P1OUT |= color2;
      __delay_cycles(FLASH_CYCLES);
    #endif
  }
}

// I2C callback functions.
void start_cb() {}

void transmit_cb(unsigned char volatile *value)
{
  *value = i2cTxBuffer;
  i2cTxBuffer = 0;
}

void receive_cb(unsigned char value) {
  if (value == SERIAL_START_BYTE) {
    i2cRxCursor = 1;
  } else if (i2cRxCursor == 1) {
    i2cRxBuffer[0] = value;
    i2cRxCursor = 2;
  } else if (i2cRxCursor == 2 && value == SERIAL_STOP_BYTE) {
    // received outOfEnergy.
    outOfEnergy = i2cRxBuffer[0];
    i2cRxCursor = 0;
  } else if (i2cRxCursor == 2) {
    i2cRxBuffer[1] = value;
    i2cRxCursor = 3;
  } else if (i2cRxCursor == 3 && value == SERIAL_STOP_BYTE) {
    // received team color and ownTaggerCode.
    teamColor = i2cRxBuffer[0] << 3;
    ownTaggerCode = i2cRxBuffer[1];
    i2cRxCursor = 0;
    flashLed(COLOR_WHITE, teamColor, 1);
  } 
}

void initI2C (void) {
  // I2C adress is determined by the 3 jumpers on P2.0 P2.1 P2.3.
  #ifdef IS_TAGGER_RECEIVER
    slave_i2c_init(start_cb, transmit_cb, receive_cb, (0x68));
  #elif
    char domeId = P2IN & 0x7;
    slave_i2c_init(start_cb, transmit_cb, receive_cb, (I2C_BASE_ADRESS + domeId));
  #endif
}

/*
 * main.c
 */
int main(void) {
  initClocks();
  initIOPins();
  __enable_interrupt();
  initI2C();
  flashLed(COLOR_WHITE, COLOR_BLACK, 1);

  while (1) {
    P1OUT &= ~COLOR_WHITE;
    P1OUT |= teamColor;
    // flash back and white if out of energy.
    while (outOfEnergy) {
      flashLed(COLOR_WHITE, COLOR_BLACK, 1);
      irValid = 0;
    }

    // Got hit?
    if (irValid) {
      // Check the parity and stop bit.
      // If irParityCheck is 1, the parity bit is wrong.
      if (irDataBuffer & 0x01 && ~irParityCheck & 0x01) {
        if ((irDataBuffer >> 2) != ownTaggerCode) {
          i2cTxBuffer = irDataBuffer >> 2;
          flashLed(COLOR_WHITE, teamColor, 3);
        }
      }
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
  __delay_cycles(IR_START_BIT_SECURIT_LENGTH);
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

// USCI RX interrupt for uart and i2c.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector=USCIAB0RX_VECTOR
  __interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIORX_ISR (void)
#else
  #error Compiler not supported!
#endif
{
  i2cRxInterrupt();
  #ifdef DEBUG_UART
    serialInterrupt();
  #endif
}

// USCI TX interrupt for i2c.
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector = USCIAB0TX_VECTOR
  __interrupt void usci_i2c_data_isr (void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) usci_i2c_data_isr (void)
#else
  #error Compiler not supported!
#endif
{
  i2cTxInterrupt();
}
