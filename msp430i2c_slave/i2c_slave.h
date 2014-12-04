#ifndef USCI_LIB
#define USCI_LIB

#define SDA_PIN BIT7                                  // msp430g2553 UCB0SDA pin
#define SCL_PIN BIT6                                  // msp430g2553 UCB0SCL pin

void slave_i2c_receive_init(unsigned char slave_address, unsigned int prescale);
void slave_i2c_receive_init(unsigned char slave_address, unsigned int prescale);
void slave_i2c_receive(unsigned char byteCount, unsigned char *field);
void slave_i2c_transmit(unsigned char byteCount, unsigned char *field, unsigned char restart);
unsigned char slave_i2c_slave_present(unsigned char slave_address);
unsigned char i2c_ready();


#endif
