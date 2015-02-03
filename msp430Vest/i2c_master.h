#ifndef USCI_LIB
#define USCI_LIB

#define SDA_PIN BIT7                                  // msp430g2553 UCB0SDA pin
#define SCL_PIN BIT6                                  // msp430g2553 UCB0SCL pin

void master_i2c_receive_init(unsigned char slave_address, unsigned int prescale);
void master_i2c_transmit_init(unsigned char slave_address, unsigned int prescale);
void master_i2c_receive(unsigned char byteCount, unsigned char *field);
void master_i2c_transmit(unsigned char byteCount, unsigned char *field);
unsigned char master_i2c_slave_present(unsigned char slave_address);
unsigned char i2c_ready();
void master_i2c_receive_interrupt();
void master_i2c_transmit_interrupt();

#endif
