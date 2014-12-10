//******************************************************************************
//   MSP430 USCI I2C Transmitter and Receiver
//
//  Description: This code configures the MSP430's USCI module as
//  I2C master capable of transmitting and receiving bytes.
//******************************************************************************

#include "msp430g2553.h"                        // device specific header
#include "i2c_master.h"
#define SDA_PIN BIT7                                  // msp430g2553 UCB0SDA pin
#define SCL_PIN BIT6                                  // msp430g2553 UCB0SCL pin

// i2c variables
signed char byteCtr;
unsigned char *TI_receive_field;
unsigned char *TI_transmit_field;

// uart variables
#define RXBUFFERSIZE 32

// Serial ringbuffer & bufferpositions
char volatile rxBuffer[RXBUFFERSIZE];
char volatile rxBufferStart = 0;
char volatile rxBufferEnd = 0;
// Error flag for serial buffer
char rxBufferError = 0;
// Echo flag - 1 if we should echo back the serial input
char echoBack = 0;


//------------------------------------------------------------------------------
// void TI_USCI_I2C_receiveinit(unsigned char slave_address,
//                              unsigned char prescale)
//-----------------------------------------------------------------------------
void master_i2c_receive_init(unsigned char slave_address,
                             unsigned int prescale){
  P1SEL |= SDA_PIN + SCL_PIN;                 // Assign I2C pins to USCI_B0
  P1SEL2 |= SDA_PIN + SCL_PIN;
  UCB0CTL1 = UCSWRST;                        // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
  UCB0BR0 = (unsigned char)(prescale & 0xFF); // set prescaler
  UCB0BR1 = (unsigned char)((prescale >> 8) & 0xFF);
  UCB0I2CSA = slave_address;                  // set slave address
  UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
  UCB0I2CIE = UCNACKIE;
  IE2 = UCB0RXIE;                            // Enable RX interrupt
}

//------------------------------------------------------------------------------
// void TI_USCI_I2C_transmitinit(unsigned char slave_address,
//                               unsigned char prescale)
//------------------------------------------------------------------------------
void master_i2c_transmit_init(unsigned char slave_address,
                          unsigned int prescale){
  P1SEL |= SDA_PIN + SCL_PIN;                 // Assign I2C pins to USCI_B0
  P1SEL2 |= SDA_PIN + SCL_PIN;
  UCB0CTL1 = UCSWRST;                        // Enable SW reset
  UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;       // I2C Master, synchronous mode
  UCB0CTL1 = UCSSEL_2 + UCSWRST;              // Use SMCLK, keep SW reset
  UCB0BR0 = (unsigned char)(prescale & 0xFF); // set prescaler
  UCB0BR1 = (unsigned char)((prescale >> 8) & 0xFF);
  UCB0I2CSA = slave_address;                  // Set slave address
  UCB0CTL1 &= ~UCSWRST;                       // Clear SW reset, resume operation
  UCB0I2CIE = UCNACKIE;
  IE2 = UCB0TXIE;                            // Enable TX ready interrupt
}

//------------------------------------------------------------------------------
// void TI_USCI_I2C_receive(unsigned char byteCount, unsigned char *field)
//------------------------------------------------------------------------------
void master_i2c_receive(unsigned char byteCount, unsigned char *field){
  TI_receive_field = field;
  if ( byteCount == 1 ){
    byteCtr = 0 ;
    __disable_interrupt();
    UCB0CTL1 |= UCTXSTT;                      // I2C start condition
    while (UCB0CTL1 & UCTXSTT);
    UCB0CTL1 |= UCTXSTP;                      // I2C stop condition
    __enable_interrupt();
  } else if ( byteCount > 1 ) {
    byteCtr = byteCount - 2 ;
    UCB0CTL1 |= UCTXSTT;                      // I2C start condition
  } else
    while (1);                                // illegal parameter
}

//------------------------------------------------------------------------------
// void TI_USCI_I2C_transmit(unsigned char byteCount, unsigned char *field)
//------------------------------------------------------------------------------
void master_i2c_transmit(unsigned char byteCount, unsigned char *field){
  TI_transmit_field = field;
  byteCtr = byteCount;
  UCB0CTL1 |= UCTR + UCTXSTT;                 // I2C TX, start condition
}

//------------------------------------------------------------------------------
// unsigned char TI_USCI_I2C_slave_present(unsigned char slave_address)
//------------------------------------------------------------------------------
unsigned char master_i2c_slave_present(unsigned char slave_address){
  unsigned char ie2_bak, slaveadr_bak, ucb0i2cie, returnValue;
  ucb0i2cie = UCB0I2CIE;                      // restore old UCB0I2CIE
  ie2_bak = IE2;                              // store IE2 register
  slaveadr_bak = UCB0I2CSA;                   // store old slave address
  UCB0I2CIE &= ~ UCNACKIE;                    // no NACK interrupt
  UCB0I2CSA = slave_address;                  // set slave address
  IE2 &= ~(UCB0TXIE + UCB0RXIE);              // no RX or TX interrupts
  __disable_interrupt();
  UCB0CTL1 |= UCTR + UCTXSTT + UCTXSTP;       // I2C TX, start condition
  while (UCB0CTL1 & UCTXSTP);                 // wait for STOP condition

  returnValue = !(UCB0STAT & UCNACKIFG);
  __enable_interrupt();
  IE2 = ie2_bak;                              // restore IE2
  UCB0I2CSA = slaveadr_bak;                   // restore old slave address
  UCB0I2CIE = ucb0i2cie;                      // restore old UCB0CTL1
  return returnValue;                         // return whether or not
                                              // a NACK occured
}

//------------------------------------------------------------------------------
// unsigned char TI_USCI_I2C_ready()
//------------------------------------------------------------------------------
unsigned char i2c_ready(){
  return !(UCB0STAT & UCBBUSY);
}

// UART functions
// 1 if serial input should be echoed again, 0 if not
void serialEchoBack(char e)
{
  echoBack = (!e)?0:1;
}

// Returns 1 if there's been an overflow, 0 if not.
char serialError()
{
  char r = rxBufferError;
  rxBufferError = 0;
  return r;
}

// Print one character (given by <tx>) on UART
void serialWrite(char tx)
{
  // TX buffer ready?
  while (!(IFG2&UCA0TXIFG));
  // Print byte
  UCA0TXBUF = tx;
  // Wait for transmit
  while (!(IFG2&UCA0TXIFG));
}

// Print integer as readable number to UART (using ASCII)
// Ignore leading zeros.
void serialPrintInt(int i)
{
  int j = i;
  if (j > 10000)
    serialWrite(0x30 + i/10000);
  i = i % 10000;
  if (j > 1000)
    serialWrite(0x30 + i/1000);
  i = i % 1000;
  if (j > 100)
    serialWrite(0x30 + i/100);
  i = i % 100;
  if (j > 10)
    serialWrite(0x30 + i/10);
  i = i % 10;
  serialWrite(0x30 + i/1);
}

// Print chars given by <tx> on UART
void serialPrint(char* tx)
{
  int b, i = 0;
  while(tx[i] != 0x00)
    i++;
  for (b = 0; b < i; b++)
    serialWrite(tx[b]);
}

// Print chars given by <tx> on UART and print \n afterwards.
void serialPrintln(char* tx)
{
  // Print <tx>
  serialPrint(tx);
  // Print \n\r
  serialWrite(0x0D);
  serialWrite(0x0A);
}

// Return 1 if there's serial data available
char serialAvailable(void)
{
  if (rxBufferStart != rxBufferEnd)
    return 1;
  return 0;
}

// Clear all received bytes.
void serialFlush(void)
{
  rxBufferStart = rxBufferEnd;
}

// Returns the first byte from the ringbuffer but does not delete it. Returns -1 if no data is available.
int serialPeek(void)
{
  if (rxBufferStart == rxBufferEnd)
    return -1;
  return rxBuffer[rxBufferStart];
}

// Returns the first byte from the ringbuffer and deletes it. Returns -1 if no data is available.
int serialRead(void)
{
  if (rxBufferStart == rxBufferEnd)
    return -1;
  char r = rxBuffer[rxBufferStart++];
  rxBufferStart %= RXBUFFERSIZE;
  return r;
}

// Reads in a number from the serialInterface
// This is a very basic implementation and you might want to write your own
// depending on your scenario
int serialReadInt(void)
{
  int number = 0;
  char stop = 0;
  char negative = 0;
  while (!stop)
  {
    while (!serialAvailable());
    char letter = serialRead();
    if (letter == '-' && number == 0)
      negative = 1;
    else if (letter >= '0' && letter <= '9')
      number = number * 10 + (letter - '0');
    else
      stop = 1;
  }
  if (negative)
    return number * -1;
  return number;
}

// interrupt routines

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector = USCIAB0RX_VECTOR
  __interrupt void USCIAB0RX_ISR(void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIAB0RX_ISR (void)
#else
  #error Compiler not supported!
#endif
{
  if (UCB0STAT & UCNACKIFG){            // send STOP if slave sends NACK
    UCB0CTL1 |= UCTXSTP;
    UCB0STAT &= ~UCNACKIFG;
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector = USCIAB0TX_VECTOR
  __interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
  #error Compiler not supported!
#endif
{
  // interrupts in module B (i2c)
  if (IFG2 & UCB0RXIFG){
    if ( byteCtr == 0 ){
      UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
      *TI_receive_field = UCB0RXBUF;
      TI_receive_field++;
    }
    else {
      *TI_receive_field = UCB0RXBUF;
      TI_receive_field++;
      byteCtr--;
    }
  }
  else {
    if (byteCtr == 0){
      UCB0CTL1 |= UCTXSTP;                    // I2C stop condition
      IFG2 &= ~UCB0TXIFG;                     // Clear USCI_B0 TX int flag
    }
    else {
      UCB0TXBUF = *TI_transmit_field;
      TI_transmit_field++;
      byteCtr--;
    }
  }

  // interrupts in module A (UART)
  if (IFG2 & UCB0RXIFG)
  {
    // Store received byte in ringbuffer
    rxBuffer[rxBufferEnd++] = UCA0RXBUF;
    rxBufferEnd %= RXBUFFERSIZE;
    if (echoBack)
    {
      while (!(IFG2&UCA0TXIFG));
      UCA0TXBUF = UCA0RXBUF;
    }
    // Check for an overflow
    if (rxBufferStart == rxBufferEnd)
      rxBufferError = 1;
  }
}
