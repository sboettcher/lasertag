#include <msp430.h>
//#include "TI_USCI_I2C_master.h"


void initPins()
{
	WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer

  _EINT();
}

/*
 * Main function
 */
int main(void)
{
  initPins();

  unsigned char data[] = {0x3B};
  unsigned char indata[10];

  while(1)
  {
    /*
    TI_USCI_I2C_transmitinit(0x68, 10);
    while(!TI_USCI_I2C_ready());
    TI_USCI_I2C_transmit(1, data, 1);
    //while(!TI_USCI_I2C_ready());
    */

    TI_USCI_I2C_receiveinit(0x68, 10);
    while(!TI_USCI_I2C_ready());
    TI_USCI_I2C_receive(4, indata, 1);
    while(!TI_USCI_I2C_ready());


  }

  return 0;
}
