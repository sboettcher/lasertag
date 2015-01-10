#include <msp430.h>
#include "i2c_master.h"

/**
 * The vest is bluetooth slave and i2c master to communicate between the tagger
 * and the domes.
 * The domes are addressed within 0x60 and 0x67 and the number of domes must be 
 * known.
 */

#define I2C_BASE_ADRESS 0x60
#define I2C_PRESCALE 1600
#define BT_START_BYTE 0xFF
#define NUM_DOMES 1

// The vest id is used for the bluetooth name and is sent to the domes to avoid
// getting hit by the own tagger.
const unsigned char vestId = 1;

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
  UCA0BR0 = (417 & 0xFF);                   // 16MHz 38400 (required by bluetooth module)
  UCA0BR1 = (417 >> 8);                     // 16MHz 38400
  UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
  UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine
  IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

void setupBlueToothSlave() {
  serialPrint("\r\n+STWMOD=0\r\n");//set the bluetooth work in master mode
  //set the bluetooth name as "SeeedBTMaster"
  serialPrint("\r\n+STNA=vest");
  serialPrint(vestId);
  serialPrint("\r\n");
  serialPrint("\r\n+STPIN=0000\r\n");//Set Master pincode"0000",it must be same as Slave pincode
  serialPrint("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  serialPrint("\r\n+STAUTO=0\r\n");// Auto-connection is forbidden here
  __delay_cycles(32000000); // This delay is required.
  serialPrint("\r\n+INQ=1\r\n");//make the master inquire
  serialPrint("Vest bluetooth is inquirable!");
  __delay_cycles(32000000); // This delay is required.
  serialFlush();
}

void sendIdToDomes() {
  for (i = 0; i < NUM_DOMES; i++) {
    master_i2c_transmit_init(I2C_BASE_ADRESS + i, I2C_PRESCALE);
    while(!i2c_ready());
    master_i2c_transmit(1, &vestId);
    while(!i2c_ready());
  }
}

int main(void) {
  initClocks();
  initUART();
  _EINT();

  setupBlueToothSlave();

  sendIdToDomes();

  int i = 0;
  unsigned char* indata;

  while (1) {
    // Loop through the connected domes and ask for received hits.
    for (i = 0; i < NUM_DOMES; i++) {
      master_i2c_receive_init(I2C_BASE_ADRESS + i, I2C_PRESCALE);
      while(!i2c_ready());
      master_i2c_receive(1, indata);
      while(!i2c_ready());

      // Send received hit codes that are different from 0 and the dome number.
      if (*indata != 0) {
        serialPrint(BT_START_BYTE);
        serialPrint(indata);
        serialPrint(i);
      }
    }
  }
  return 0;
}
