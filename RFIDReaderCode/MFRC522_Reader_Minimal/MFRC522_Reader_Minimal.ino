/*
 * Copyright: Universität Freiburg, 2015
 * Author: Marc Pfeifer <pfeiferm@tf.uni-freiburg.de>
 *
 * Based on code from the MFRC522 arduino library, writen by Dr.Leong, Miguel Balboa, Søren Thing Andersen and Tom Clement.
 * (https://github.com/miguelbalboa/rfid)
 *
 * A minimal arduino firmware to read only the uid of an MIRFARE tag with an MFRC522-RFID-reader.
 */

#include <SPI.h>

// Struct for the UID.
struct Uid {
  byte size;			    // Number of bytes in the UID. 4, 7 or 10.
  byte uidByte[10];   // The UID as bytes.
  byte sak;			      // The SAK (Select acknowledge) byte returned from the PICC after successful selection.
};

// All needed defines.
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

// The UID.
Uid uid;

/*
 * The setup method. Initializes everything.
 */
void setup() {
  Serial.begin(9600);		// Initialize serial communications with the PC
  SPI.begin();			// Init SPI bus
  Serial.println("Hallo!");
  initSPI();	// Init the SPI connection to the reader.
  PCD_Init();	                // Init MFRC522
  Serial.println("Scan PICC to see UID");
}

/*
 * The main loop. Checks for new card, selects one and prints the UID. 
 */
void loop() {
  // Look for new cards
  if ( !PICC_IsNewCardPresent()) {
    return;
  }
  // Select one of the cards
  if ( !PICC_ReadCardSerial()) {
    return;
  }
  // Print the UID.
  printCardID();
}
  
/*
 * Initialization of the SPI communication with the reader.
 */
void initSPI() {
  // Set the chipSelectPin as digital output, do not select the slave yet.
  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH);	
  // Set the resetPowerDownPin as digital output, do not reset or power down.
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, LOW);
  // Set SPI bus to work with MFRC522 chip.
  setSPIConfig();
}

/*
 * Initialization of the reader.
 */
void PCD_Init() {
  if (digitalRead(RST_PIN) == LOW) {	//The MFRC522 chip is in power down mode.
    digitalWrite(RST_PIN, HIGH);		// Exit power down mode. This triggers a hard reset.
    // Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74µs. Let us be generous: 50ms.
    delay(50);
  } else { // Perform a soft reset
    PCD_Reset();
  }
  // When communicating with a PICC we need a timeout if something goes wrong.
  // f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
  // TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
  PCD_WriteRegister(TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
  PCD_WriteRegister(TPrescalerReg, 0xA9);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25µs.
  PCD_WriteRegister(TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
  PCD_WriteRegister(TReloadRegL, 0xE8);
  PCD_WriteRegister(TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
  PCD_WriteRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
  PCD_AntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
}

/*
 * Resets the reader.
 */
void PCD_Reset() {
  PCD_WriteRegister(CommandReg, PCD_SoftReset);	// Issue the SoftReset command.
  // The datasheet does not mention how long the SoftRest command takes to complete.
  // But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg) 
  // Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74�s. Let us be generous: 50ms.
  delay(50);
  // Wait for the PowerDown bit in CommandReg to be cleared
  while (PCD_ReadRegister(CommandReg) & (1<<4)) {
    // PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
  }
}

/*
 * Activate the antenna of the reader.
 */
void PCD_AntennaOn() {
  byte value = PCD_ReadRegister(TxControlReg);
  if ((value & 0x03) != 0x03) {
    PCD_WriteRegister(TxControlReg, value | 0x03);
  }
}

/*
 * Configure the SPI communication.
 */
void setSPIConfig() {
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
}

/*
 * Check if a new tag is availavle.
 */
bool PICC_IsNewCardPresent() {
  byte bufferATQA[2];
  byte bufferSize = sizeof(bufferATQA);
  byte result = PICC_RequestA(bufferATQA, &bufferSize);
  return (result == STATUS_OK || result == STATUS_COLLISION);
}

/*
 * Request something at the reader.
 */
byte PICC_RequestA(byte *bufferATQA, byte *bufferSize) {
  return PICC_REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
}

/*
 * Request something at the reader.
 */
byte PICC_REQA_or_WUPA( byte command, byte *bufferATQA, byte *bufferSize) {
  byte validBits;
  byte status;	
  if (bufferATQA == NULL || *bufferSize < 2) {	// The ATQA response is 2 bytes long.
	return STATUS_NO_ROOM;
  }
  PCD_ClearRegisterBitMask(CollReg, 0x80);		// ValuesAfterColl=1 => Bits received after collision are cleared.
  PCD_ReadRegister(CollReg);
  validBits = 7;									// For REQA and WUPA we need the short frame format - transmit only 7 bits of the last (and only) byte. TxLastBits = BitFramingReg[2..0]
  status = PCD_TransceiveData(&command, 1, bufferATQA, bufferSize, &validBits);
  if (status != STATUS_OK) {
    return status;
  }
  if (*bufferSize != 2 || validBits != 0) {		// ATQA must be exactly 16 bits.
    return STATUS_ERROR;
  }
  return STATUS_OK;
}

/*
 * Clears the bit mask.
 */
void PCD_ClearRegisterBitMask(byte reg, byte mask) {
  byte tmp;
  tmp = PCD_ReadRegister(reg);
  PCD_WriteRegister(reg, tmp & (~mask));		// clear bit mask
}

/*
 * Read a given register in the reader.
 */
byte PCD_ReadRegister(byte reg) {
	byte value;
	digitalWrite(SS_PIN, LOW);			// Select slave
	SPI.transfer(0x80 | (reg & 0x7E));			// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	value = SPI.transfer(0);					// Read the value back. Send 0 to stop reading.
	digitalWrite(SS_PIN, HIGH);			// Release slave again
	return value;
}

/*
 * Read a given register in the reader.
 */
void PCD_ReadRegister(byte reg, byte count, byte *values, byte rxAlign) {
	if (count == 0) {
		return;
	}
	byte address = 0x80 | (reg & 0x7E);		// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	byte index = 0;							// Index in values array.
	digitalWrite(SS_PIN, LOW);		// Select slave
	count--;								// One read is performed outside of the loop
	SPI.transfer(address);					// Tell MFRC522 which address we want to read
	while (index < count) {
		if (index == 0 && rxAlign) {		// Only update bit positions rxAlign..7 in values[0]
			// Create bit mask for bit positions rxAlign..7
			byte mask = 0;
			for (byte i = rxAlign; i <= 7; i++) {
				mask |= (1 << i);
			}
			// Read value and tell that we want to read the same address again.
			byte value = SPI.transfer(address);	
			// Apply mask to both current value of values[0] and the new data in value.
			values[0] = (values[index] & ~mask) | (value & mask);
		}
		else { // Normal case
			values[index] = SPI.transfer(address);	// Read value and tell that we want to read the same address again.
		}
		index++;
	}
	values[index] = SPI.transfer(0);			// Read the final byte. Send 0 to stop reading.
	digitalWrite(SS_PIN, HIGH);			// Release slave again
}

/*
 * Write a given register in the reader.
 */
void PCD_WriteRegister(byte reg, byte value) {
	digitalWrite(SS_PIN, LOW);		// Select slave
	SPI.transfer(reg & 0x7E);	        // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
	SPI.transfer(value);
	digitalWrite(SS_PIN, HIGH);		// Release slave again
}

/*
 * Write a given register in the reader.
 */
void PCD_WriteRegister(	byte reg, byte count, byte *values) {
	digitalWrite(SS_PIN, LOW);		// Select slave
	SPI.transfer(reg & 0x7E);				// MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
	for (byte index = 0; index < count; index++) {
		SPI.transfer(values[index]);
	}
	digitalWrite(SS_PIN, HIGH);		// Release slave again
}

/*
 * Communicates with the tag.
 */
byte PCD_TransceiveData( byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits) {
	byte waitIRq = 0x30;		// RxIRq and IdleIRq
	return PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, 0, false);
}

/*
 * Communicates with the tag.
 */
byte PCD_CommunicateWithPICC(byte command, byte waitIRq, byte *sendData, byte sendLen, byte *backData, byte *backLen, byte *validBits, byte rxAlign, bool checkCRC) {
	byte n, _validBits;
	unsigned int i;

	// Prepare values for BitFramingReg
	byte txLastBits = validBits ? *validBits : 0;
	byte bitFraming	= (rxAlign << 4) + txLastBits;		// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
	
	PCD_WriteRegister(CommandReg, PCD_Idle);			// Stop any active command.
	PCD_WriteRegister(ComIrqReg, 0x7F);					// Clear all seven interrupt request bits
	PCD_SetRegisterBitMask(FIFOLevelReg, 0x80);			// FlushBuffer = 1, FIFO initialization
	PCD_WriteRegister(FIFODataReg, sendLen, sendData);	// Write sendData to the FIFO
	PCD_WriteRegister(BitFramingReg, bitFraming);		// Bit adjustments
	PCD_WriteRegister(CommandReg, command);				// Execute the command
	if (command == PCD_Transceive) {
		PCD_SetRegisterBitMask(BitFramingReg, 0x80);	// StartSend=1, transmission of data starts
	}
	
	// Wait for the command to complete.
	// In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
	// Each iteration of the do-while-loop takes 17.86�s.
	i = 2000;
	while (1) {
		n = PCD_ReadRegister(ComIrqReg);	// ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
		if (n & waitIRq) {					// One of the interrupts that signal success has been set.
			break;
		}
		if (n & 0x01) {						// Timer interrupt - nothing received in 25ms
			return STATUS_TIMEOUT;
		}
		if (--i == 0) {						// The emergency break. If all other condions fail we will eventually terminate on this one after 35.7ms. Communication with the MFRC522 might be down.
			return STATUS_TIMEOUT;
		}
	}
	
	// Stop now if any errors except collisions were detected.
	byte errorRegValue = PCD_ReadRegister(ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
	if (errorRegValue & 0x13) {	 // BufferOvfl ParityErr ProtocolErr
		return STATUS_ERROR;
	}	

	// If the caller wants data back, get it from the MFRC522.
	if (backData && backLen) {
		n = PCD_ReadRegister(FIFOLevelReg);			// Number of bytes in the FIFO
		if (n > *backLen) {
			return STATUS_NO_ROOM;
		}
		*backLen = n;											// Number of bytes returned
		PCD_ReadRegister(FIFODataReg, n, backData, rxAlign);	// Get received data from FIFO
		_validBits = PCD_ReadRegister(ControlReg) & 0x07;		// RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
		if (validBits) {
			*validBits = _validBits;
		}
	}
	
	// Tell about collisions
	if (errorRegValue & 0x08) {		// CollErr
		return STATUS_COLLISION;
	}
	
	// Perform CRC_A validation if requested.
	if (backData && backLen && checkCRC) {
		// In this case a MIFARE Classic NAK is not OK.
		if (*backLen == 1 && _validBits == 4) {
			return STATUS_MIFARE_NACK;
		}
		// We need at least the CRC_A value and all 8 bits of the last byte must be received.
		if (*backLen < 2 || _validBits != 0) {
			return STATUS_CRC_WRONG;
		}
		// Verify CRC_A - do our own calculation and store the control in controlBuffer.
		byte controlBuffer[2]; 
		n = PCD_CalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
		if (n != STATUS_OK) {
			return n;
		}
		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1])) {
			return STATUS_CRC_WRONG;
		}
	}
	
	return STATUS_OK;
}

/*
 * Sets a register bis mask.
 */
void PCD_SetRegisterBitMask(byte reg, byte mask) { 
	byte tmp;
	tmp = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, tmp | mask);			// set bit mask
}

/*
 * Calculates the CRC-checksum.
 */
byte PCD_CalculateCRC(byte *data, byte length, byte *result) {
	PCD_WriteRegister(CommandReg, PCD_Idle);		// Stop any active command.
	PCD_WriteRegister(DivIrqReg, 0x04);				// Clear the CRCIRq interrupt request bit
	PCD_SetRegisterBitMask(FIFOLevelReg, 0x80);		// FlushBuffer = 1, FIFO initialization
	PCD_WriteRegister(FIFODataReg, length, data);	// Write data to the FIFO
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);		// Start the calculation
	
	// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73�s.
	word i = 5000;
	byte n;
	while (1) {
		n = PCD_ReadRegister(DivIrqReg);	// DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
		if (n & 0x04) {						// CRCIRq bit set - calculation done
			break;
		}
		if (--i == 0) {						// The emergency break. We will eventually terminate on this one after 89ms. Communication with the MFRC522 might be down.
			return STATUS_TIMEOUT;
		}
	}
	PCD_WriteRegister(CommandReg, PCD_Idle);		// Stop calculating CRC for new content in the FIFO.
	
	// Transfer the result from the registers to the result buffer
	result[0] = PCD_ReadRegister(CRCResultRegL);
	result[1] = PCD_ReadRegister(CRCResultRegH);
	return STATUS_OK;
}

/*
 * Select a tag.
 */
bool PICC_ReadCardSerial() {
  byte result = PICC_Select(0);
  return (result == STATUS_OK);
}

/*
 * Select a tag.
 */
byte PICC_Select(byte validBits) {
  bool uidComplete;
  bool selectDone;
  bool useCascadeTag;
  byte cascadeLevel	= 1; 
  byte result;
  byte count;
  byte index;
  byte uidIndex;					// The first index in uid->uidByte[] that is used in the current Cascade Level.
  char currentLevelKnownBits;		// The number of known UID bits in the current Cascade Level.
  byte buffer[9];					// The SELECT/ANTICOLLISION commands uses a 7 byte standard frame + 2 bytes CRC_A
  byte bufferUsed;				// The number of bytes used in the buffer, ie the number of bytes to transfer to the FIFO.
  byte rxAlign;					// Used in BitFramingReg. Defines the bit position for the first bit received.
  byte txLastBits;				// Used in BitFramingReg. The number of valid bits in the last transmitted byte. 
  byte *responseBuffer;
  byte responseLength;
	
  // Description of buffer structure:
  //		Byte 0: SEL 				Indicates the Cascade Level: PICC_CMD_SEL_CL1, PICC_CMD_SEL_CL2 or PICC_CMD_SEL_CL3	//		Byte 1: NVB					Number of Valid Bits (in complete command, not just the UID): High nibble: complete bytes, Low nibble: Extra bits. 
  //		Byte 2: UID-data or CT		See explanation below. CT means Cascade Tag.
  //		Byte 3: UID-data
  //		Byte 4: UID-data
  //		Byte 5: UID-data
  //		Byte 6: BCC					Block Check Character - XOR of bytes 2-5
  //		Byte 7: CRC_A
  //		Byte 8: CRC_A
  // The BCC and CRC_A is only transmitted if we know all the UID bits of the current Cascade Level.
  //
  // Description of bytes 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft: UID contents and cascade levels)
  //		UID size	Cascade level	Byte2	Byte3	Byte4	Byte5
  //		========	=============	=====	=====	=====	=====
  //		 4 bytes		1			uid0	uid1	uid2	uid3
  //		 7 bytes		1			CT		uid0	uid1	uid2
  //						2			uid3	uid4	uid5	uid6
  //		10 bytes		1			CT		uid0	uid1	uid2
  //						2			CT		uid3	uid4	uid5
  //						3			uid6	uid7	uid8	uid9
	
  // Sanity checks
  if (validBits > 80) {
  	return STATUS_INVALID;
  }

  // Prepare MFRC522
  PCD_ClearRegisterBitMask(CollReg, 0x80);		// ValuesAfterColl=1 => Bits received after collision are cleared.

  // Repeat Cascade Level loop until we have a complete UID.
  uidComplete = false;
  while ( ! uidComplete) {
    // Set the Cascade Level in the SEL byte, find out if we need to use the Cascade Tag in byte 2.
    switch (cascadeLevel) {
      case 1:
        buffer[0] = PICC_CMD_SEL_CL1;
        uidIndex = 0;
        useCascadeTag = validBits && uid.size > 4;	// When we know that the UID has more than 4 bytes
        break;
        case 2:
          buffer[0] = PICC_CMD_SEL_CL2;
          uidIndex = 3;
          useCascadeTag = validBits && uid.size > 7;	// When we know that the UID has more than 7 bytes
          break;
        case 3:
          buffer[0] = PICC_CMD_SEL_CL3;
          uidIndex = 6;
          useCascadeTag = false;						// Never used in CL3.
          break;
        default:
	  return STATUS_INTERNAL_ERROR;
	  break;
    }
		
    // How many UID bits are known in this Cascade Level?
    currentLevelKnownBits = validBits - (8 * uidIndex);
    if (currentLevelKnownBits < 0) {
      currentLevelKnownBits = 0;
    }
    // Copy the known bits from uid->uidByte[] to buffer[]
    index = 2; // destination index in buffer[]
    if (useCascadeTag) {
      buffer[index++] = PICC_CMD_CT;
    }
    byte bytesToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1 : 0); // The number of bytes needed to represent the known bits for this level.
    if (bytesToCopy) {
      byte maxBytes = useCascadeTag ? 3 : 4; // Max 4 bytes in each Cascade Level. Only 3 left if we use the Cascade Tag
      if (bytesToCopy > maxBytes) { 
        bytesToCopy = maxBytes;
      }
      for (count = 0; count < bytesToCopy; count++) {
        buffer[index++] = uid.uidByte[uidIndex + count];
      }
    }
    // Now that the data has been copied we need to include the 8 bits in CT in currentLevelKnownBits
    if (useCascadeTag) {
      currentLevelKnownBits += 8;
    }
    // Repeat anti collision loop until we can transmit all UID bits + BCC and receive a SAK - max 32 iterations.
    selectDone = false;
    while ( ! selectDone) {
      // Find out how many bits and bytes to send and receive.
      if (currentLevelKnownBits >= 32) { // All UID bits in this Cascade Level are known. This is a SELECT.
        //Serial.print("SELECT: currentLevelKnownBits="); Serial.println(currentLevelKnownBits, DEC);
        buffer[1] = 0x70; // NVB - Number of Valid Bits: Seven whole bytes
        // Calculate BCC - Block Check Character
        buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
        // Calculate CRC_A
        result = PCD_CalculateCRC(buffer, 7, &buffer[7]);
        if (result != STATUS_OK) {
          return result;
        }
        txLastBits = 0; // 0 => All 8 bits are valid.
        bufferUsed = 9;
        // Store response in the last 3 bytes of buffer (BCC and CRC_A - not needed after tx)
        responseBuffer	= &buffer[6];
        responseLength	= 3;
      } else { // This is an ANTICOLLISION.
        //Serial.print("ANTICOLLISION: currentLevelKnownBits="); Serial.println(currentLevelKnownBits, DEC);
        txLastBits		= currentLevelKnownBits % 8;
        count			= currentLevelKnownBits / 8;	// Number of whole bytes in the UID part.
        index			= 2 + count;					// Number of whole bytes: SEL + NVB + UIDs
        buffer[1]		= (index << 4) + txLastBits;	// NVB - Number of Valid Bits
        bufferUsed		= index + (txLastBits ? 1 : 0);
        // Store response in the unused part of buffer
        responseBuffer	= &buffer[index];
        responseLength	= sizeof(buffer) - index;
      }
      // Set bit adjustments
      rxAlign = txLastBits;											// Having a seperate variable is overkill. But it makes the next line easier to read.
      PCD_WriteRegister(BitFramingReg, (rxAlign << 4) + txLastBits);	// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
      // Transmit the buffer and receive the response.
      // result = PCD_TransceiveData(buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign);			
      result = PCD_CommunicateWithPICC(PCD_Transceive, 0x30, buffer, bufferUsed, responseBuffer, &responseLength, &txLastBits, rxAlign, false);
      if (result == STATUS_COLLISION) { // More than one PICC in the field => collision.
        result = PCD_ReadRegister(CollReg); // CollReg[7..0] bits are: ValuesAfterColl reserved CollPosNotValid CollPos[4:0]  
        if (result & 0x20) { // CollPosNotValid
          return STATUS_COLLISION; // Without a valid collision position we cannot continue
        }
        byte collisionPos = result & 0x1F; // Values 0-31, 0 means bit 32.
        if (collisionPos == 0) {
          collisionPos = 32;
        }
        if (collisionPos <= currentLevelKnownBits) { // No progress - should not happen 
          return STATUS_INTERNAL_ERROR;
        }
        // Choose the PICC with the bit set.
        currentLevelKnownBits = collisionPos;
        count			= (currentLevelKnownBits - 1) % 8; // The bit to modify
        index			= 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0); // First byte is index 0.
        buffer[index]	|= (1 << count); 
      } else if (result != STATUS_OK) {
        return result;
      } else { // STATUS_OK
        if (currentLevelKnownBits >= 32) { // This was a SELECT.
          selectDone = true; // No more anticollision 
          // We continue below outside the while.
        } else { // This was an ANTICOLLISION.
          // We now have all 32 bits of the UID in this Cascade Level
          currentLevelKnownBits = 32;
          // Run loop again to do the SELECT.
        }
      }
    } // End of while ( ! selectDone)
    // We do not check the CBB - it was constructed by us above.
    // Copy the found UID bytes from buffer[] to uid->uidByte[]
    index			= (buffer[2] == PICC_CMD_CT) ? 3 : 2; // source index in buffer[]
    bytesToCopy		= (buffer[2] == PICC_CMD_CT) ? 3 : 4;
    for (count = 0; count < bytesToCopy; count++) {
      uid.uidByte[uidIndex + count] = buffer[index++];
    }
    // Check response SAK (Select Acknowledge)
    if (responseLength != 3 || txLastBits != 0) {		// SAK must be exactly 24 bits (1 byte + CRC_A).
      return STATUS_ERROR;
    }
    // Verify CRC_A - do our own calculation and store the control in buffer[2..3] - those bytes are not needed anymore.
    result = PCD_CalculateCRC(responseBuffer, 1, &buffer[2]);
    if (result != STATUS_OK) {
      return result;
    }
    if ((buffer[2] != responseBuffer[1]) || (buffer[3] != responseBuffer[2])) {
      return STATUS_CRC_WRONG;
    }
    if (responseBuffer[0] & 0x04) { // Cascade bit set - UID not complete yes
       cascadeLevel++;
     } else {
       uidComplete = true;
       uid.sak = responseBuffer[0];
     }
  } // End of while ( ! uidComplete)
  // Set correct uid->size
  uid.size = 3 * cascadeLevel + 1;
  return STATUS_OK;
}

/*
 * Print out the UID of tag.
 */
void printCardID() {
  Serial.print("Card UID:");
  for (byte i = 0; i < uid.size; i++) {
    Serial.print(uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(uid.uidByte[i], HEX);
  } 
  Serial.println();
}
