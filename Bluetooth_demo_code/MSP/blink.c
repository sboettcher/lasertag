#include <msp430.h>
#include <templateEMP.h>
#include <string.h>


volatile int nameIndex = 0;
volatile int addrIndex = 0;

char recvBuf[200];
char* slaveAddr;
char* slaveName = ";jse_note";


char* retSymb = "+RTINQ=";//start symble when there's any return
char connectCmd[50] = "\r\n+CONN=";

int indexOf(const char* searchString, const char* pattern, const int offset) {
	  int i;
	  volatile int sucess = 0;
	  // If the pattern is longer than the string to search for, return -1.
	  if (strlen(pattern) > strlen(searchString))
		  return -1;
	  // Search for the pattern.
	  for (i = offset; i < strlen(searchString); i++) {
		  if (searchString[i] == pattern[sucess])
			  sucess += 1;
		  else {
		      sucess = 0;
		  }
		  // We've found the pattern!
		  if (sucess == strlen(pattern))
			  return i - sucess + 1;
	  }
	  // Pattern not found
	  return -1;
}

char* subString(const char* string, const int from, const int to) {
	// Allocate some memory for the new String.
	char* sString = malloc(to - from + 2);
	// Write into the new String
	int i;
	for (i = 0; i < to - from + 1; i++)
		sString[i] = string[i + from];
    // Terminating 0
    sString[to - from + 1] = 0;
	return sString;
}


void setupBlueToothConnection() {
  serialPrint("\r\n+STWMOD=1\r\n");//set the bluetooth work in master mode
  //serialPrint("\r\n+STBD=9600\r\n"); // Set the baudrate to 9600
  //changeBaud();
  serialPrint("\r\n+STNA=SeeedBTMaster\r\n");//set the bluetooth name as "SeeedBTMaster"
  serialPrint("\r\n+STPIN=0000\r\n");//Set Master pincode"0000",it must be same as Slave pincode
  serialPrint("\r\n+STAUTO=0\r\n");// Auto-connection is forbidden here
  //serialPrintln("Size of Int on MSP: ");
  //serialPrintInt(sizeof(int));
  __delay_cycles(2000000); // This delay is required.
  serialFlush();
  serialPrint("\r\n+INQ=1\r\n");//make the master inquire
  serialPrint("Master is inquiring!");
  __delay_cycles(2000000); // This delay is required.
  int pos = 0;
  //find the target slave
  int firstTry = 1;
  char recvChar;
  //serialPrint(printf("%i", test));
  while(1){
    if(serialAvailable()){
      if (firstTry) {
	    firstTry = 0;
	    recvBuf[pos++] = serialRead();
      }
      else {
    	recvBuf[pos++] = serialRead();
        recvBuf[pos] = 0;
      }
    }
    nameIndex = indexOf(recvBuf, slaveName, 0);//get the position of slave name
    if ( nameIndex != -1 && nameIndex > 0){
		nameIndex -= 1;//decrease the ';' in front of the slave name, to get the position of the end of the slave address
	    addrIndex = indexOf(recvBuf, retSymb, (nameIndex - strlen(retSymb) - 18)) + strlen(retSymb);
	    slaveAddr = subString(recvBuf, addrIndex, nameIndex);
        break;
      }
  }

  /*
   * Now, that we've found the slave address, connect to the slave.
   */
  serialPrintln("Inquiring completed, sending command :");
  serialPrintln("connectCmd");
  strcat(connectCmd, slaveAddr);
  strcat(connectCmd, "\r\n");
  int connected = 0;
  firstTry = 1;
  pos = 0;

  while (!connected) {
	  serialPrint(connectCmd);//send connection command
	  while(1){
		  if(serialAvailable()){
			  if (firstTry) {
				  firstTry = 0;
			      recvBuf[pos++] = serialRead();
  	          } else {
  	        	  recvBuf[pos++] = serialRead();
  	        	  recvBuf[pos] = 0;
  	        }
  	      }
  	      if (indexOf(recvBuf, "CONNECT:OK", 0) != -1) {  //Try to find the OK
  	    	  connected = 1;
  	          serialPrintln("Connected!");
  	          break;
  	      } else if (indexOf(recvBuf, "CONNECT:FAIL", 0) != -1) {
  	    	  break;
  	      }
	  }
  }
}

int main(void) {
  initMSP();
  WDTCTL = WDTPW + WDTHOLD;		// Stop watchdog timer
  P1DIR |= 0x01;					// Set P1.0 to output direction

  setupBlueToothConnection();
  while (1) {
	  serialPrintln("abcdefg");
	  serialPrintln("1234567");
  }
  return 0;
}
