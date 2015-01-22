#ifndef USCI_LIB
#define USCI_LIB

#define SDA_PIN BIT7                                  // msp430g2553 UCB0SDA pin
#define SCL_PIN BIT6                                  // msp430g2553 UCB0SCL pin


void slave_i2c_init(void (*SCallback)(), void (*TCallback)(unsigned char volatile *value), void (*RCallback)(unsigned char value), unsigned char slave_address);

void inline i2cTxInterrupt(void);

void i2cRxInterrupt(void);

#endif
