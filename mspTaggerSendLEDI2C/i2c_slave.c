//******************************************************************************
//   MSP430 USCI I2C Transmitter and Receiver
//
//  Description: This code configures the MSP430's USCI module as
//  I2C slave capable of transmitting and receiving bytes.
//******************************************************************************

#include "msp430g2553.h"                        // device specific header
#include "i2c_slave.h"

void (*i2c_receive_callback)(unsigned char receive);
void (*i2c_transmit_callback)(unsigned char volatile *send_next);
void (*i2c_start_callback)(void);


void slave_i2c_init(void (*SCallback)(), void (*TCallback)(unsigned char volatile *value), void (*RCallback)(unsigned char value), unsigned char slave_address)
{
  P1SEL |= SDA_PIN + SCL_PIN; // Assign I2C pins to USCI_B0
  P1SEL2 |= SDA_PIN + SCL_PIN; // Assign I2C pins to USCI_B0
  UCB0CTL1 |= UCSWRST; // Enable SW reset
  UCB0CTL0 = UCMODE_3 + UCSYNC; // I2C Slave, synchronous mode
  UCB0I2COA = slave_address; // set own (slave) address
  UCB0CTL1 &= ~UCSWRST; // Clear SW reset, resume operation
  IE2 |= UCB0TXIE + UCB0RXIE; // Enable TX interrupt
  UCB0I2CIE |= UCSTTIE; // Enable STT interrupt
  i2c_start_callback = SCallback;
  i2c_receive_callback = RCallback;
  i2c_transmit_callback = TCallback;
}


void inline i2cTxInterrupt(void)
{
  if (IFG2 & UCB0TXIFG)
    i2c_transmit_callback(&UCB0TXBUF);
  else
    i2c_receive_callback(UCB0RXBUF);
}


void inline i2cRxInterrupt(void)
{
  UCB0STAT &= ~UCSTTIFG; // Clear start condition int flag
  i2c_start_callback();
}
