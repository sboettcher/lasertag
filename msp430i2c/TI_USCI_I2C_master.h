#ifndef USCI_LIB
#define USCI_LIB

#define SDA_PIN BIT7                                  // msp430g2553 UCB0SDA pin
#define SCL_PIN BIT6                                  // msp430g2553 UCB0SCL pin

void TI_USCI_I2C_receiveinit(unsigned char slave_address, unsigned int prescale);
void TI_USCI_I2C_transmitinit(unsigned char slave_address, unsigned int prescale);


void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field);
void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field, unsigned char restart);


unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address);
unsigned char TI_USCI_I2C_ready();


#endif
