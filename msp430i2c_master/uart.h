#define RXBUFFERSIZE 32

// Serial ringbuffer & bufferpositions
char volatile rxBuffer[RXBUFFERSIZE];
char volatile rxBufferStart = 0;
char volatile rxBufferEnd = 0;
// Error flag for serial buffer
char rxBufferError = 0;
// Echo flag - 1 if we should echo back the serial input
char echoBack = 0;


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

void uart_receive_interrupt()
{
  // interrupts in module A (UART)
  if (IFG2 & UCA0RXIFG)
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
