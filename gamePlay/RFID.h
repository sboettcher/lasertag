#ifndef RFID_H
#define RFID_H
#include <SoftwareSerial.h>
#include <SPI.h>
#include "config.h"
#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


struct Uid {
  byte size;			// Number of bytes in the UID. 4, 7 or 10.
  byte uidByte[10];
  byte sak;			// The SAK (Select acknowledge) byte returned from the PICC after successful selection.
};

#define RST_PIN	A2
#define SS_PIN 7
#define PICC_CMD_REQA 0x26
#define STATUS_OK 1	// Success
#define	STATUS_ERROR 2      // Error in communication
#define STATUS_NO_ROOM 5
#define CollReg 0x0E << 1
#define PCD_Transceive 0x0C
#define CommandReg 0x01 << 1
#define PCD_Idle 0x00
#define ComIrqReg 0x04 << 1
#define FIFODataReg 0x09 << 1
#define FIFOLevelReg 0x0A << 1
#define BitFramingReg 0x0D << 1
#define STATUS_TIMEOUT 4
#define ErrorReg 0x06 << 1
#define ControlReg 0x0C << 1
#define STATUS_COLLISION 3	// Collission detected
#define STATUS_INTERNAL_ERROR 6	// Internal error in the code. Should not happen ;-)
#define STATUS_INVALID 7	// Invalid argument.
#define STATUS_CRC_WRONG 8	// The CRC_A does not match
#define STATUS_MIFARE_NACK 9		// A MIFARE PICC responded with NAK
#define DivIrqReg 0x05 << 1
#define PCD_CalcCRC 0x03
#define CRCResultRegH 0x21 << 1	// shows the MSB and LSB values of the CRC calculation
#define CRCResultRegL 0x22 << 1
#define PICC_CMD_SEL_CL1 0x93		// Anti collision/Select, Cascade Level 1
#define PICC_CMD_SEL_CL2 0x95		// Anti collision/Select, Cascade Level 1
#define PICC_CMD_SEL_CL3 0x97		// Anti collision/Select, Cascade Level 1
#define PICC_CMD_CT 0x88
#define TModeReg 0x2A << 1
#define TPrescalerReg 0x2B << 1	// the lower 8 bits of the TPrescaler value. The 4 high bits are in TModeReg.
#define TReloadRegH 0x2C << 1	// defines the 16-bit timer reload value
#define TReloadRegL 0x2D << 1
#define TxASKReg 0x15 << 1
#define ModeReg 0x11 << 1
#define PCD_SoftReset 0x0F
#define TxControlReg 0x14 << 1


class RFID {
public:
  // Constructor
  RFID();
  
  // Start RFID reader with SPI or with serial
  void begin();
  
  // Look if command availalble
  bool available();
  
  // Returns the tag number, if some available
  long getTag();
  
private:
  Uid uid;
  
  void initSPI();
  void PCD_Init();
  void PCD_Reset();
  void PCD_AntennaOn();
  void setSPIConfig();
  bool PICC_IsNewCardPresent();
  byte PICC_RequestA(byte *bufferATQA, byte *bufferSize);
  byte PICC_REQA_or_WUPA( byte command, byte *bufferATQA, byte *bufferSize);
  void PCD_ClearRegisterBitMask(byte reg, byte mask);
  byte PCD_ReadRegister(byte reg);
  void PCD_ReadRegister(byte reg, byte count, byte *values, byte rxAlign);
  void PCD_WriteRegister(byte reg, byte value);
  void PCD_WriteRegister(	byte reg, byte count, byte *values);
  byte PCD_TransceiveData( byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits);
  byte PCD_CommunicateWithPICC(byte command, byte waitIRq, byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits, byte rxAlign, bool checkCRC);
  void PCD_SetRegisterBitMask(byte reg, byte mask);
  byte PCD_CalculateCRC(byte *data, byte length, byte *result);
  bool PICC_ReadCardSerial();
  byte PICC_Select(byte validBits);
  void printCardID();
};

#endif
