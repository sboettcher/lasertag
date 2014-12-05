#ifndef USCI_LIB
#define USCI_LIB

#define SDA_PIN BIT7                                  // msp430g2553 UCB0SDA pin
#define SCL_PIN BIT6                                  // msp430g2553 UCB0SCL pin


void TI_USCI_I2C_slaveinit(void (*SCallback)(), void (*TCallback)(unsigned char volatile *value), void (*RCallback)(unsigned char value), unsigned char slave_address);

#endif
