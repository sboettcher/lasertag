// Copyright 2015 Sebastian Boettcher

#include "./MFRC522.h"


MFRC522::MFRC522(int rstpin, int cspin) {
  _spi = new mraa::Spi(0);
  
  mraa_result_t response;
  
  // init reset gpio
  _rst = new mraa::Gpio(rstpin);
  if (_rst == NULL) {
    mraa::printError(MRAA_ERROR_UNSPECIFIED);
  }
  response = _rst->dir(mraa::DIR_OUT);
  if (response != MRAA_SUCCESS) {
    mraa::printError(response);
  }
  
  // init chip select gpio
  _cs = new mraa::Gpio(cspin);
  if (_cs == NULL) {
    mraa::printError(MRAA_ERROR_UNSPECIFIED);
  }
  response = _cs->dir(mraa::DIR_OUT);
  if (response != MRAA_SUCCESS) {
    mraa::printError(response);
  }
  
  initSPI();	
  PCD_Init();
}

MFRC522::~MFRC522() {
	delete _spi;
	delete _rst;
	delete _cs;
}


void MFRC522::CS(int cs) {
  mraa_result_t response = _cs->write(cs);
  if (response != MRAA_SUCCESS) {
    mraa::printError(response);
  }
}
void MFRC522::RST(int rst) {
  mraa_result_t response = _rst->write(rst);
  if (response != MRAA_SUCCESS) {
    mraa::printError(response);
  }
}

void MFRC522::initSPI() {
  // init spi
  _spi->lsbmode(0);
  _spi->mode(mraa::SPI_MODE1);
  _spi->frequency(25000000);
  
  // Set the chipSelectPin as digital output, do not select the slave yet
  // _chipSelectPin = SS_PIN;
  CS(1);
  
  // Set the resetPowerDownPin as digital output, do not reset or power down.
  // _resetPowerDownPin = resetPowerDownPin;
  RST(0);
}

void MFRC522::PCD_Init() {
  
  if (_rst->read() == 0) {	//The MFRC522 chip is in power down mode.
    RST(1); // Exit power down mode. This triggers a hard reset.
    // Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74�s. Let us be generous: 50ms.
    usleep(50000);
  } else { // Perform a soft reset
    PCD_Reset();
  }
  // When communicating with a PICC we need a timeout if something goes wrong.
  // f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
  // TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
  PCD_WriteRegister(TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
  PCD_WriteRegister(TPrescalerReg, 0xA9);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25�s.
  PCD_WriteRegister(TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
  PCD_WriteRegister(TReloadRegL, 0xE8);
  PCD_WriteRegister(TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
  PCD_WriteRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
  PCD_AntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
} // End PCD_Init()



void MFRC522::PCD_Reset() {
  PCD_WriteRegister(CommandReg, PCD_SoftReset);	// Issue the SoftReset command.
  // The datasheet does not mention how long the SoftRest command takes to complete.
  // But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg) 
  // Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74�s. Let us be generous: 50ms.
  usleep(50000);
  // Wait for the PowerDown bit in CommandReg to be cleared
  while (PCD_ReadRegister(CommandReg) & (1<<4)) {
    // PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
  }
} // End PCD_Reset()

void MFRC522::PCD_AntennaOn() {
  uint8_t value = PCD_ReadRegister(TxControlReg);
  if ((value & 0x03) != 0x03) {
    PCD_WriteRegister(TxControlReg, value | 0x03);
  }
} // End PCD_AntennaOn()


bool MFRC522::PICC_IsNewCardPresent() {
  uint8_t bufferATQA[2];
  uint8_t bufferSize = sizeof(bufferATQA);
  uint8_t result = PICC_RequestA(bufferATQA, &bufferSize);
  return (result == STATUS_OK || result == STATUS_COLLISION);
}

uint8_t MFRC522::PICC_RequestA(uint8_t *bufferATQA, uint8_t *bufferSize) {
  return PICC_REQA_or_WUPA(PICC_CMD_REQA, bufferATQA, bufferSize);
}

uint8_t MFRC522::PICC_REQA_or_WUPA(uint8_t command, uint8_t *bufferATQA, uint8_t *bufferSize) {
  uint8_t validBits;
  uint8_t status;	
  if (bufferATQA == NULL || *bufferSize < 2) {	// The ATQA response is 2 uint8_ts long.
	return STATUS_NO_ROOM;
  }
  PCD_ClearRegisterBitMask(CollReg, 0x80);		// ValuesAfterColl=1 => Bits received after collision are cleared.
  PCD_ReadRegister(CollReg);
  validBits = 7;									// For REQA and WUPA we need the short frame format - transmit only 7 bits of the last (and only) uint8_t. TxLastBits = BitFramingReg[2..0]
  status = PCD_TransceiveData(&command, 1, bufferATQA, bufferSize, &validBits);
  if (status != STATUS_OK) {
    return status;
  }
  if (*bufferSize != 2 || validBits != 0) {		// ATQA must be exactly 16 bits.
    return STATUS_ERROR;
  }
  return STATUS_OK;
}

void MFRC522::PCD_ClearRegisterBitMask(uint8_t reg, uint8_t mask) {
  uint8_t tmp;
  tmp = PCD_ReadRegister(reg);
  PCD_WriteRegister(reg, tmp & (~mask));		// clear bit mask
}



uint8_t MFRC522::PCD_ReadRegister(uint8_t reg) {
	uint8_t value;

  CS(0); // Select slave

  uint8_t tx_buf[2];
  uint8_t rx_buf[2];
  tx_buf[0] = 0x80 | (reg & 0x7E);
  tx_buf[1] = 0;
  _spi->transfer(tx_buf, rx_buf, 2);
  value = rx_buf[1];
  
	//SPI.transfer(0x80 | (reg & 0x7E));			// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	//value = SPI.transfer(0);					// Read the value back. Send 0 to stop reading.
	
  CS(1); // Release slave again

	return value;
}

void MFRC522::PCD_ReadRegister(uint8_t reg, uint8_t count, uint8_t *values, uint8_t rxAlign) {
	if (count == 0) {
		return;
	}
	uint8_t address = 0x80 | (reg & 0x7E);		// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	//uint8_t index = 0;							// Index in values array.
	CS(0);		// Select slave
  
  uint8_t tx_buf[count];
  uint8_t rx_buf[count];
  
  for (uint8_t i = 0; i < count; ++i)
    tx_buf[i] = address;
  
	//count--;								// One read is performed outside of the loop
	//SPI.transfer(address);					// Tell MFRC522 which address we want to read
  _spi->transfer(tx_buf, rx_buf, count);
  if (rxAlign) {
    uint8_t mask = 0;
		for (uint8_t i = rxAlign; i <= 7; i++) {
			mask |= (1 << i);
		}
    rx_buf[0] = (rx_buf[0] & ~mask) | (rx_buf[1] & mask);
  }
  values = rx_buf;
  
	//while (index < count) {
	//	if (index == 0 && rxAlign) {		// Only update bit positions rxAlign..7 in values[0]
	//		// Create bit mask for bit positions rxAlign..7
	//		uint8_t mask = 0;
	//		for (uint8_t i = rxAlign; i <= 7; i++) {
	//			mask |= (1 << i);
	//		}
	//		// Read value and tell that we want to read the same address again.
	//		// uint8_t value = SPI.transfer(address);	
	//		// Apply mask to both current value of values[0] and the new data in value.
	//		rx_buf[0] = (rx_buf[index] & ~mask) | (rx_buf[1] & mask);
	//	} else { // Normal case
	//		//values[index] = SPI.transfer(address);	// Read value and tell that we want to read the same address again.
	//	}
	//	index++;
	//}
	//values[index] = SPI.transfer(0);			// Read the final uint8_t. Send 0 to stop reading.
	
  CS(1);			// Release slave again
} // End PCD_ReadRegister()

void MFRC522::PCD_WriteRegister(uint8_t reg, uint8_t value) {
	CS(0);		// Select slave
  uint8_t buf[2];
  buf[0] = reg & 0x7E; // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
  buf[1] = value;
  _spi->write(buf, 2);
	CS(1);		// Release slave again
} // End PCD_WriteRegister()

void MFRC522::PCD_WriteRegister(	uint8_t reg, uint8_t count, uint8_t *values) {
	CS(0);		// Select slave
	uint8_t buf[count+1];
  buf[0] = reg & 0x7E; // MSB == 0 is for writing. LSB is not used in address. Datasheet section 8.1.2.3.
  for (uint8_t i = 0; i < count; ++i)
    buf[i+1] = values[i];
  _spi->write(buf, count+1);
	CS(1);		// Release slave again
} // End PCD_WriteRegister()

uint8_t MFRC522::PCD_TransceiveData( uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits) {
	uint8_t waitIRq = 0x30;		// RxIRq and IdleIRq
	return PCD_CommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, 0, false);
} // End PCD_TransceiveData()


uint8_t MFRC522::PCD_CommunicateWithPICC(uint8_t command, uint8_t waitIRq, uint8_t *sendData, uint8_t sendLen, uint8_t *backData, uint8_t *backLen, uint8_t *validBits, uint8_t rxAlign, bool checkCRC) {
	uint8_t n, _validBits;
	unsigned int i;

	// Prepare values for BitFramingReg
	uint8_t txLastBits = validBits ? *validBits : 0;
	uint8_t bitFraming	= (rxAlign << 4) + txLastBits;		// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]
	
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
	uint8_t errorRegValue = PCD_ReadRegister(ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
	if (errorRegValue & 0x13) {	 // BufferOvfl ParityErr ProtocolErr
		return STATUS_ERROR;
	}	

	// If the caller wants data back, get it from the MFRC522.
	if (backData && backLen) {
		n = PCD_ReadRegister(FIFOLevelReg);			// Number of uint8_ts in the FIFO
		if (n > *backLen) {
			return STATUS_NO_ROOM;
		}
		*backLen = n;											// Number of uint8_ts returned
		PCD_ReadRegister(FIFODataReg, n, backData, rxAlign);	// Get received data from FIFO
		_validBits = PCD_ReadRegister(ControlReg) & 0x07;		// RxLastBits[2:0] indicates the number of valid bits in the last received uint8_t. If this value is 000b, the whole uint8_t is valid.
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
		// We need at least the CRC_A value and all 8 bits of the last uint8_t must be received.
		if (*backLen < 2 || _validBits != 0) {
			return STATUS_CRC_WRONG;
		}
		// Verify CRC_A - do our own calculation and store the control in controlBuffer.
		uint8_t controlBuffer[2]; 
		n = PCD_CalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
		if (n != STATUS_OK) {
			return n;
		}
		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1])) {
			return STATUS_CRC_WRONG;
		}
	}
	
	return STATUS_OK;
} // End PCD_CommunicateWithPICC()

void MFRC522::PCD_SetRegisterBitMask(uint8_t reg, uint8_t mask) { 
	uint8_t tmp;
	tmp = PCD_ReadRegister(reg);
	PCD_WriteRegister(reg, tmp | mask);			// set bit mask
} // End PCD_SetRegisterBitMask()

uint8_t MFRC522::PCD_CalculateCRC(uint8_t *data, uint8_t length, uint8_t *result) {
	PCD_WriteRegister(CommandReg, PCD_Idle);		// Stop any active command.
	PCD_WriteRegister(DivIrqReg, 0x04);				// Clear the CRCIRq interrupt request bit
	PCD_SetRegisterBitMask(FIFOLevelReg, 0x80);		// FlushBuffer = 1, FIFO initialization
	PCD_WriteRegister(FIFODataReg, length, data);	// Write data to the FIFO
	PCD_WriteRegister(CommandReg, PCD_CalcCRC);		// Start the calculation
	
	// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73�s.
	uint16_t i = 5000;
	uint8_t n;
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
} // End PCD_CalculateCRC()

bool MFRC522::PICC_ReadCardSerial() {
  uint8_t result = PICC_Select(0);
  return (result == STATUS_OK);
} // End PICC_ReadCardSerial()



// uint8_t PICC_Select(Uid *uid, uint8_t validBits) {
uint8_t MFRC522::PICC_Select(uint8_t validBits) {
  bool uidComplete;
  bool selectDone;
  bool useCascadeTag;
  uint8_t cascadeLevel	= 1; 
  uint8_t result;
  uint8_t count;
  uint8_t index;
  uint8_t uidIndex;					// The first index in uid->uidByte[] that is used in the current Cascade Level.
  char currentLevelKnownBits;		// The number of known UID bits in the current Cascade Level.
  uint8_t buffer[9];					// The SELECT/ANTICOLLISION commands uses a 7 uint8_t standard frame + 2 uint8_ts CRC_A
  uint8_t bufferUsed;				// The number of uint8_ts used in the buffer, ie the number of uint8_ts to transfer to the FIFO.
  uint8_t rxAlign;					// Used in BitFramingReg. Defines the bit position for the first bit received.
  uint8_t txLastBits;				// Used in BitFramingReg. The number of valid bits in the last transmitted uint8_t. 
  uint8_t *responseBuffer;
  uint8_t responseLength;
	
  // Description of buffer structure:
  //		uint8_t 0: SEL 				Indicates the Cascade Level: PICC_CMD_SEL_CL1, PICC_CMD_SEL_CL2 or PICC_CMD_SEL_CL3	//		uint8_t 1: NVB					Number of Valid Bits (in complete command, not just the UID): High nibble: complete uint8_ts, Low nibble: Extra bits. 
  //		uint8_t 2: UID-data or CT		See explanation below. CT means Cascade Tag.
  //		uint8_t 3: UID-data
  //		uint8_t 4: UID-data
  //		uint8_t 5: UID-data
  //		uint8_t 6: BCC					Block Check Character - XOR of uint8_ts 2-5
  //		uint8_t 7: CRC_A
  //		uint8_t 8: CRC_A
  // The BCC and CRC_A is only transmitted if we know all the UID bits of the current Cascade Level.
  //
  // Description of uint8_ts 2-5: (Section 6.5.4 of the ISO/IEC 14443-3 draft: UID contents and cascade levels)
  //		UID size	Cascade level	uint8_t2	uint8_t3	uint8_t4	uint8_t5
  //		========	=============	=====	=====	=====	=====
  //		 4 uint8_ts		1			uid0	uid1	uid2	uid3
  //		 7 uint8_ts		1			CT		uid0	uid1	uid2
  //						2			uid3	uid4	uid5	uid6
  //		10 uint8_ts		1			CT		uid0	uid1	uid2
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
    // Set the Cascade Level in the SEL uint8_t, find out if we need to use the Cascade Tag in uint8_t 2.
    switch (cascadeLevel) {
      case 1:
        buffer[0] = PICC_CMD_SEL_CL1;
        uidIndex = 0;
        useCascadeTag = validBits && _uid.size > 4;	// When we know that the UID has more than 4 uint8_ts
        break;
        case 2:
          buffer[0] = PICC_CMD_SEL_CL2;
          uidIndex = 3;
          useCascadeTag = validBits && _uid.size > 7;	// When we know that the UID has more than 7 uint8_ts
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
    uint8_t uint8_tsToCopy = currentLevelKnownBits / 8 + (currentLevelKnownBits % 8 ? 1 : 0); // The number of uint8_ts needed to represent the known bits for this level.
    if (uint8_tsToCopy) {
      uint8_t maxuint8_ts = useCascadeTag ? 3 : 4; // Max 4 uint8_ts in each Cascade Level. Only 3 left if we use the Cascade Tag
      if (uint8_tsToCopy > maxuint8_ts) { 
        uint8_tsToCopy = maxuint8_ts;
      }
      for (count = 0; count < uint8_tsToCopy; count++) {
        buffer[index++] = _uid.uidByte[uidIndex + count];
      }
    }
    // Now that the data has been copied we need to include the 8 bits in CT in currentLevelKnownBits
    if (useCascadeTag) {
      currentLevelKnownBits += 8;
    }
    // Repeat anti collision loop until we can transmit all UID bits + BCC and receive a SAK - max 32 iterations.
    selectDone = false;
    while ( ! selectDone) {
      // Find out how many bits and uint8_ts to send and receive.
      if (currentLevelKnownBits >= 32) { // All UID bits in this Cascade Level are known. This is a SELECT.
        //Serial.print("SELECT: currentLevelKnownBits="); Serial.println(currentLevelKnownBits, DEC);
        buffer[1] = 0x70; // NVB - Number of Valid Bits: Seven whole uint8_ts
        // Calculate BCC - Block Check Character
        buffer[6] = buffer[2] ^ buffer[3] ^ buffer[4] ^ buffer[5];
        // Calculate CRC_A
        result = PCD_CalculateCRC(buffer, 7, &buffer[7]);
        if (result != STATUS_OK) {
          return result;
        }
        txLastBits = 0; // 0 => All 8 bits are valid.
        bufferUsed = 9;
        // Store response in the last 3 uint8_ts of buffer (BCC and CRC_A - not needed after tx)
        responseBuffer	= &buffer[6];
        responseLength	= 3;
      } else { // This is an ANTICOLLISION.
        //Serial.print("ANTICOLLISION: currentLevelKnownBits="); Serial.println(currentLevelKnownBits, DEC);
        txLastBits		= currentLevelKnownBits % 8;
        count			= currentLevelKnownBits / 8;	// Number of whole uint8_ts in the UID part.
        index			= 2 + count;					// Number of whole uint8_ts: SEL + NVB + UIDs
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
        uint8_t collisionPos = result & 0x1F; // Values 0-31, 0 means bit 32.
        if (collisionPos == 0) {
          collisionPos = 32;
        }
        if (collisionPos <= currentLevelKnownBits) { // No progress - should not happen 
          return STATUS_INTERNAL_ERROR;
        }
        // Choose the PICC with the bit set.
        currentLevelKnownBits = collisionPos;
        count			= (currentLevelKnownBits - 1) % 8; // The bit to modify
        index			= 1 + (currentLevelKnownBits / 8) + (count ? 1 : 0); // First uint8_t is index 0.
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
    // Copy the found UID uint8_ts from buffer[] to uid->uidByte[]
    index			= (buffer[2] == PICC_CMD_CT) ? 3 : 2; // source index in buffer[]
    uint8_tsToCopy		= (buffer[2] == PICC_CMD_CT) ? 3 : 4;
    for (count = 0; count < uint8_tsToCopy; count++) {
      _uid.uidByte[uidIndex + count] = buffer[index++];
    }
    // Check response SAK (Select Acknowledge)
    if (responseLength != 3 || txLastBits != 0) {		// SAK must be exactly 24 bits (1 uint8_t + CRC_A).
      return STATUS_ERROR;
    }
    // Verify CRC_A - do our own calculation and store the control in buffer[2..3] - those uint8_ts are not needed anymore.
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
       _uid.sak = responseBuffer[0];
     }
  } // End of while ( ! uidComplete)
  // Set correct uid->size
  _uid.size = 3 * cascadeLevel + 1;
  return STATUS_OK;
} // End PICC_Select()

void MFRC522::printCardID() {
  printf("Card UID: ");
  for (uint8_t i = 0; i < _uid.size; i++) {
	std::cout << (_uid.uidByte[i] < 0x10 ? " 0" : " ") << std::hex << _uid.uidByte[i] << std::dec;
    //printf("%d", _uid.uidByte[i] < 0x10 ? " 0" : " ");
    //printf("%d", _uid.uidByte[i], HEX);
  } 
  printf("\n");
  fflush(stdout);
}
