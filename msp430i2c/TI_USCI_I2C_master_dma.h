/*** USCI master library with DMA **********************************************

Uli Kretzschmar
MSP430 Systems
Freising
*******************************************************************************/
#ifndef USCI_DMA_LIB
#define USCI_DMA_LIB

#define SDA_PIN 0x02                                  // msp430x261x UCB0SDA pin
#define SCL_PIN 0x04                                  // msp430x261x UCB0SCL pin

void TI_USCI_I2C_DMA_receiveinit(unsigned char slave_address,
                             unsigned char prescale);
void TI_USCI_I2C_DMA_transmitinit( unsigned char slave_address,
                               unsigned char prescale);



void TI_USCI_I2C_DMA_receive(unsigned char byteCount, unsigned char *field);
void TI_USCI_I2C_DMA_transmit(unsigned char byteCount, unsigned char *field);

unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address);
unsigned char TI_USCI_I2C_notready();


#endif
