#include <msp430.h>
#include "i2c_master.h"


void initPins()
{
	WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer

  _EINT();

  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL  = CALDCO_16MHZ;

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

    master_i2c_receive_init(0x68, 32);
    while(!i2c_ready());
    master_i2c_receive(4, indata);
    while(!i2c_ready());


  }

  return 0;
}
