#include <msp430.h>
#include "TI_USCI_I2C_master.h"

#define DEBUG_UART
#ifdef DEBUG_UART
  #include "laserTagUART.h"
#endif

void initPins()
{
	WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

  _EINT();

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
  #endif
}

/*
 * Main function
 */
int main(void)
{
  initPins();

  #ifdef DEBUG_UART
    serialPrint("I2C Initialized!\n");
  #endif

  TI_USCI_I2C_transmitinit(0x48, 0x3f);
  while( TI_USCI_I2C_notready() );
  char data = 0xFF;
  TI_USCI_I2C_transmit(1, &data);


  return 0;
}
