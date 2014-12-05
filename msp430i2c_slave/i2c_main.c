#include <msp430.h>
#include "i2c_slave.h"

void initPins()
{
	WDTCTL = WDTPW + WDTHOLD;               // Stop watchdog timer

  _EINT();
}

void start_cb()
{
}

void transmit_cb(unsigned char volatile *value)
{
  *value = 0x42;
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
  slave_i2c_init(start_cb, transmit_cb, receive_cb, 0x02);

  return 0;
}
