#include "west.h"

// ___________________________________________________________________
West::West(HardwareSerial *serial) {
  _westSerialHW = serial;
}

// ___________________________________________________________________
void West::begin(int baudRate, int id) {
  setupBlueToothConnection(baudRate, id);
}

// ___________________________________________________________________
bool West::available() {
  if(_westSerialHW->available()>=3){//check if there's any data sent from the remote bluetooth shield
      return true;
  }
  return false;
}


// ___________________________________________________________________
Hit West::getCode() {
  // TODO what if more than one target hitted
  struct hit _hit;
  byte recvChar;
  recvChar = _westSerialHW->read(); 
  if (recvChar == 0xff) {
    _hit.code = _westSerialHW->read(); 
    _hit.position = _westSerialHW->read();  
  } 
  return _hit;
}

// ___________________________________________________________________
void West::setTeamColorRGB(uint8_t R, uint8_t G, uint8_t B) {
  uint8_t color = 1;
  if (R >= G && R >= B) color = 1;
  else if (G >= R && G >= B) color = 2;
  else if (B >= R && B >= G) color = 4;
  _westSerialHW->print(0xff);
  _westSerialHW->print(color);
  _westSerialHW->print(0xfe);
  /*
  _westSerial->print(R);
  _westSerial->print(G);
  _westSerial->print(B);*/
}

// ___________________________________________________________________
void West::setTeamColor(CRGB teamColor) {
  uint8_t color = 1;
  if (teamColor.red >= teamColor.green && teamColor.red >= teamColor.blue) color = 1;
  else if (teamColor.green >= teamColor.red && teamColor.green >= teamColor.blue) color = 2;
  else if (teamColor.blue >= teamColor.red && teamColor.blue >= teamColor.green) color = 4;
  _westSerialHW->print(0xff);
  _westSerialHW->print(color);
  _westSerialHW->print(0xfe);
  /*
  _westSerial->print('c');
  _westSerial->print(teamColor.red);
  _westSerial->print(teamColor.green);
  _westSerial->print(teamColor.blue);*/
}

// ___________________________________________________________________
void West::isDead(bool dead) {
  if (dead) {
    _westSerialHW->print('d');
  } else {
    _westSerialHW->print('h');
  }
}


// ___________________________________________________________________
void West::setupBlueToothConnection(int baudRate, int id)
{
  String retSymb = "+RTINQ=";//start symble when there's any return
  //String slaveName = ";SeeedBTSlave";//Set the Slave name ,caution that ';'must be included 
  String slaveName = String(";vest" + String(id, DEC));//Set the Slave name ,caution that ';'must be included
  
  int nameIndex = 0;
  int addrIndex = 0;
  
  String recvBuf;
  String slaveAddr;
  
  String connectCmd = "\r\n+CONN=";

  _westSerialHW->begin(baudRate); //Set BluetoothBee BaudRate to default baud rate 38400
  _westSerialHW->print("\r\n+STWMOD=1\r\n");//set the bluetooth work in master mode
  _westSerialHW->print("\r\n+STNA=SeeedBTMaster\r\n");//set the bluetooth name as "SeeedBTMaster"
  _westSerialHW->print("\r\n+STPIN=0000\r\n");//Set Master pincode"0000",it must be same as Slave pincode
  _westSerialHW->print("\r\n+STAUTO=0\r\n");// Auto-connection is forbidden here
  delay(2000); // This delay is required.
  _westSerialHW->flush();
  _westSerialHW->print("\r\n+INQ=1\r\n");//make the master inquire
  delay(2000); // This delay is required.
    
  //find the target slave
  char recvChar;
  while(1){
    if(_westSerialHW->available()){
      recvChar = _westSerialHW->read();
      recvBuf += recvChar;
      nameIndex = recvBuf.indexOf(slaveName);//get the position of slave name
      //nameIndex -= 1;//decrease the ';' in front of the slave name, to get the position of the end of the slave address
      if ( nameIndex != -1 ){
        //Serial.print(recvBuf);
        addrIndex = (recvBuf.indexOf(retSymb,(nameIndex - retSymb.length()- 18) ) + retSymb.length());//get the start position of slave address	 		
 	slaveAddr = recvBuf.substring(addrIndex, nameIndex);//get the string of slave address 			
 	break;
      }
    }
  }
  //form the full connection command
  connectCmd += slaveAddr;
  connectCmd += "\r\n";
  int connectOK = 0;
  //connecting the slave till they are connected
  _westSerialHW->print(connectCmd);//send connection command
  recvBuf = "";
  while(connectOK == 0){
    if(_westSerialHW->available()){
      recvChar = _westSerialHW->read();
      recvBuf += recvChar;
      if(recvBuf.indexOf("CONNECT:OK") != -1){
        connectOK = 1;
   	break;
      } else if(recvBuf.indexOf("CONNECT:FAIL") != -1){
        break;
      }
    }
  }
  delay(1000);
  while(_westSerialHW->available()) {_westSerialHW->read();}
}

