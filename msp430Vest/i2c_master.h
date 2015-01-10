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

void serialEchoBack(char e);
char serialError();
void serialWrite(char tx);
void serialPrintInt(int i);
void serialPrint(char* tx);
void serialPrintln(char* tx);
char serialAvailable(void);
void serialFlush(void);
int serialPeek(void);
int serialRead(void);
int serialReadInt(void);


#endif
