#include <msp430.h>
#include "i2c_slave.h"

#define DATA_SIZE 5

unsigned char volatile data[DATA_SIZE];
unsigned char volatile cursor = 0;

void initPins()
{
	WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer

  _EINT();

  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL  = CALDCO_16MHZ;
}

void start_cb()
{
  //cursor = 0;
}

void transmit_cb(unsigned char volatile *value)
{
  *value = data[cursor];
  cursor = cursor + 1 < DATA_SIZE ? cursor + 1 : 0;
}

void receive_cb(unsigned char value)
{
}

/*
 * Main function
 */
int main(void)
{
  initPins();
  data[0] = 'A';
  data[1] = 'B';
  data[2] = 'C';
  data[3] = 'D';
  data[4] = 'E';
  slave_i2c_init(start_cb, transmit_cb, receive_cb, 0x68);

  return 0;
}
