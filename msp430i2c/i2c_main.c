#include <msp430.h>
#include "TI_USCI_I2C_master.h"


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

  unsigned char data[] = {0x1, 0x2, 0x3, 0x4, 0x5};

  while(1)
  {
    TI_USCI_I2C_transmitinit(0x43, 10);
    while(TI_USCI_I2C_notready());
    TI_USCI_I2C_transmit(1, data);
    while(TI_USCI_I2C_notready());
  }

  return 0;
}
