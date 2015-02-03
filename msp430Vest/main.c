#include <msp430.h>
#include "i2c_master.h"
#include "uart.h"

/**
 * The vest is bluetooth slave and i2c master to communicate between the tagger
 * and the domes.
 * The domes are addressed within 0x60 and 0x67 and the number of domes must be 
 * known.
 */

#define I2C_BASE_ADRESS 0x60
#define I2C_PRESCALE 160
#define SERIAL_START_BYTE (char)0xFF
#define SERIAL_STOP_BYTE (char)0xFE
#define NUM_DOMES 4
unsigned char i2cTxBuffer[4] = {SERIAL_START_BYTE, 0, 0, SERIAL_STOP_BYTE};
unsigned char i2cIn = 0;
unsigned char uartRxCursor = 0;
char uartIn = 0;

// The vest id is used for the bluetooth name and is sent to the domes to avoid
// getting hit by the own tagger.
unsigned char vestId = 2;

void initClocks (void) {
  // Stop Watchdog Timer
  WDTCTL = WDTPW + WDTHOLD;

  // Init MCLK to 16 MHz
  if (CALBC1_16MHZ == 0xFF || CALDCO_16MHZ == 0xFF)
    while(1);
  BCSCTL1 = CALBC1_16MHZ;
  DCOCTL  = CALDCO_16MHZ;
}

void initUART()
{
  P1SEL |= BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
  P1SEL2 |= BIT1 + BIT2;                    // P1.1 = RXD, P1.2=TXD
  UCA0CTL1 |= UCSSEL_2;                     // SMCLK
  // UCA0BR0 = (417 & 0xFF);                   // 16MHz 38400 (required by bluetooth module)
  // UCA0BR1 = (417 >> 8);                     // 16MHz 38400
  UCA0BR0 = (1667 & 0xFF);                   // 16MHz 9600 (required by bluetooth module)
  UCA0BR1 = (1667 >> 8);                     // 16MHz 9600
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
  UC0IE |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

void setupBlueToothSlaveGrove() {
  serialPrint("\r\n+STWMOD=0\r\n");//set the bluetooth work in slave mode
  //set the bluetooth name as "vestIdxx"
  serialPrint("\r\n+STNA=vest");
  serialPrintInt(vestId);
  serialPrint("\r\n");
  serialPrint("\r\n+STPIN=0000\r\n");//Set slave pincode"0000",it must be same as Slave pincode
  serialPrint("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  serialPrint("\r\n+STAUTO=0\r\n");// Auto-connection is forbidden here
  __delay_cycles(32000000); // This 2s delay is required.
  serialPrint("\r\n+INQ=1\r\n");//make the slave inquire
  serialPrint("Vest bluetooth is inquirable!");
  __delay_cycles(32000000); // This 2s delay is required.
  serialFlush();
}

void setupBlueToothSlaveHC06() {
  __delay_cycles(32000000); // 2s delay to let bluetooth module boot.
  serialPrint("AT");
  __delay_cycles(16000000);
  serialPrint("AT+NAMEvest");
  serialPrintInt(vestId);
  __delay_cycles(16000000);
  serialPrint("AT+PIN0000");
}

// Send 
void sendIdAndColorToDomes(unsigned char color) {
  char i;
  i2cTxBuffer[1] = color;
  i2cTxBuffer[2] = vestId;
  for (i = 0; i < NUM_DOMES; i++) {
    master_i2c_transmit_init(I2C_BASE_ADRESS + i, I2C_PRESCALE);
    while(!i2c_ready());
    master_i2c_transmit(4, i2cTxBuffer);
    while(!i2c_ready());
  }
}

int main(void) {
  P2DIR = 0;
  initClocks();
  initUART();
  // _EINT();
  __enable_interrupt();
  setupBlueToothSlaveHC06();
  sendIdAndColorToDomes(0x00);

  char i;

  while (1) {
    // Receive team color via uart and send to domes.
    while (serialAvailable()) {
      uartIn = serialRead();

      // serialWrite(uartIn);
      // serialPrintln("test");

      if (uartIn == SERIAL_START_BYTE) {
        uartRxCursor = 1;
      } else if (uartRxCursor == 1) {
        sendIdAndColorToDomes((unsigned char) uartIn);
        uartRxCursor = 0;
      }
    }

    // Loop through the connected domes and ask for received hits.
    for (i = 0; i < NUM_DOMES; i++) {
      i2cIn = 0;
      master_i2c_receive_init(I2C_BASE_ADRESS + i, I2C_PRESCALE);
      while(!i2c_ready());
      master_i2c_receive(1, &i2cIn);
      while(!i2c_ready());

      // Send received hit codes that are different from 0 and the dome number.
      if (i2cIn != 0) {
        serialWrite(SERIAL_START_BYTE);
        serialWrite((char) i2cIn);
        serialWrite(i);
      }
    } 
  }
}

// I2C and UART RX interrupt
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector = USCIAB0RX_VECTOR
  __interrupt void USCIAB0RX_ISR(void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIAB0RX_ISR (void)
#else
  #error Compiler not supported!
#endif
{
  uart_receive_interrupt();
  master_i2c_receive_interrupt();
}

// I2C and UART TX interrupt
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
  #pragma vector = USCIAB0TX_VECTOR
  __interrupt void USCIAB0TX_ISR(void)
#elif defined(__GNUC__)
  void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCIAB0TX_ISR (void)
#else
  #error Compiler not supported!
#endif
{
  master_i2c_transmit_interrupt();
}
