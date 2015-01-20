// Copyright 2015 Sebastian Boettcher

#ifndef MFRC522_h
#define MFRC522_h

#include <mraa.hpp>

#include <string>
#include <iostream>
#include <iomanip>

#define RST_PIN	9
#define SS_PIN 10
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

struct Uid {
  uint8_t size;			// Number of uint8_ts in the UID. 4, 7 or 10.
  uint8_t uidByte[10];
  uint8_t sak;			// The SAK (Select acknowledge) uint8_t returned from the PICC after successful selection.
};



class MFRC522 {

  public:
    MFRC522(int rstpin, int cspin);
    ~MFRC522();
    
    void CS(int cs);
    void RST(int rst);
    
    void initSPI();
    void PCD_Init();
    void PCD_Reset();
    void PCD_AntennaOn();
    
    bool PICC_IsNewCardPresent();
    uint8_t PICC_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize);
    uint8_t PICC_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize);
    
    void PCD_ClearRegisterBitMask(uint8_t reg, uint8_t mask);
    uint8_t PCD_ReadRegister(uint8_t reg);
    void PCD_ReadRegister(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rxAlign);
    
    void PCD_WriteRegister(uint8_t reg, uint8_t value);
    void PCD_WriteRegister(	uint8_t reg, uint8_t count, uint8_t *values);
    
    uint8_t PCD_TransceiveData( uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits);
    uint8_t PCD_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, bool checkCRC);
    
    void PCD_SetRegisterBitMask(uint8_t reg, uint8_t mask);
    uint8_t PCD_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result);
    
    bool PICC_ReadCardSerial();
    uint8_t PICC_Select(uint8_t validBits);
    
    void printCardID();
    
  private:
  
    mraa::Spi* _spi;
    mraa::Gpio* _rst;
    mraa::Gpio* _cs;

    Uid _uid;
};

#endif
