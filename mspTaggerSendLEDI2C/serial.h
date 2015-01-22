/*
 * Benjamin Völker, University of Freiburg
 * mail: voelkerb@me.com
 */


#ifndef SERIAL_H_
#define SERIAL_H_


#include <stdio.h>


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


void serialBegin(int baudRate, int clkSpeed) {
	// Activate UART on 1.1 / 1.2
	// Fixed connection to PC, shows up there as COMx
	P1SEL = BIT1 + BIT2;                      // P1.1 = RXD, P1.2=TXD
	P1SEL2 = BIT1 + BIT2;                     // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_2;

	int baud = (int)((float)1000000.0/(float)baudRate*clkSpeed);
	UCA0BR0 = 0xff & baud;
	UCA0BR1 = baud >> 8;

	UCA0MCTL = UCBRS0;
	UCA0CTL1 &= ~UCSWRST;                     // Initialize USCI state machine

	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt

	__enable_interrupt();                     // Interrupts enabled

	serialWrite(0x0C);
	serialPrintln("MSP430 booted.");
}


#endif /* SERIAL_H_ */

