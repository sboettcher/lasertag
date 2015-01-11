#include <msp430.h>
#include "i2c_master.h"
#include "uart.h"


void initPins()
{
	WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer

  _EINT();

  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL  = CALDCO_16MHZ;

  P1SEL |= BIT1 + BIT2;                      // P1.1 = RXD, P1.2=TXD
  P1SEL2 |= BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  UCA0BR0 = (1667 & 0xFF);                   // 8MHz 9600
  UCA0BR1 = (1667 >> 8);                     // 8MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

/*
 * Main function
 */
int main(void)
{
  initPins();

  unsigned char data[] = {0x3B};
  unsigned char indata[5];
  indata[4] = '\0';

  serialPrintln("MSP I2C Master booted and sending.");

  while(1)
  {
    /*
    TI_USCI_I2C_transmitinit(0x68, 10);
    while(!TI_USCI_I2C_ready());
    TI_USCI_I2C_transmit(1, data, 1);
    //while(!TI_USCI_I2C_ready());
    */

    master_i2c_receive_init(0x50, 1600);
    //master_i2c_receive_init(0x68, 32);
    while(!i2c_ready());
    master_i2c_receive(4, indata);
    while(!i2c_ready());

    serialPrint("Got i2c data: ");
    serialPrintln(indata);


  }

  return 0;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector = USCIAB0RX_VECTOR
  __interrupt void USCIAB0RX_ISR(void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIAB0RX_ISR (void)
#else
  #error Compiler not supported!
#endif
{
  void uart_receive_interrupt();
  void master_i2c_receive_interrupt();
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector = USCIAB0TX_VECTOR
  __interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
  #error Compiler not supported!
#endif
{
  void master_i2c_transmit_interrupt();
}
