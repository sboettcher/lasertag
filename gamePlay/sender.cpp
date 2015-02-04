#include "sender.h"


// ___________________________________________________________________
IRSender::IRSender(uint8_t buttonPin, uint8_t ledPin) {
  _buttonPin = buttonPin;
  _irLEDPin = ledPin;
}

// ___________________________________________________________________
void IRSender::init(uint8_t code, int shootDelay) {
  pinMode(_buttonPin, INPUT_PULLUP);
  pinMode(_irLEDPin, OUTPUT);
  _code = code;
  _shootDelay = shootDelay;
  _sendTimer = 0;
  setupTimer();
  sendZero();
}

// ___________________________________________________________________
int IRSender::shootIfNeeded() {
  if (millis() - _sendTimer > _shootDelay) {
    _sendTimer = millis();
    if (!digitalRead(_buttonPin)) {
      noInterrupts();
      send(_code);
      interrupts();
      return 1;
    } else {
      return -1;
    }
  } else {
    if (!digitalRead(_buttonPin)) {
      return 0;
    } else {
      return -1;
    }
  }
}

// ___________________________________________________________________
void IRSender::send(uint8_t code) {
  // Start transmitting with an rising edge.
  sendOne();
  delayMicroseconds(BIT_TIME_US/2);
  // Convert the code into a byte.
  byte c = byte(code & 0x00FF);
  int highCount = 0;
  // Check for each bit in the byte if it is a 1 or a 0 and send a
  // signal correspondingly for a given time.
  
  // Loop over all Bits
  for (int i = 0; i < 8; i++) {
    // If code and Mask not 0
    if ((c & MASK) != 0) {
      // Send a logical 1
      sendOne();
      // Count the 1s for parity
      highCount++;
    } else {
      sendZero();
    }
    // Shift the Bit to send
    c = c << 1;
    delayMicroseconds(BIT_TIME_US);
  }
  /*
  for (byte mask = 1; mask > 0; mask <<= 1) {
    if (c & mask) {
      sendOne();
      // Count the 1s for parity
      highCount++;
    } else {
      sendZero();
    }
    delayMicroseconds(BIT_TIME_US);
  }*/
  // Add a parity-bit.
  if (highCount % 2 == 1) {
    sendOne();
  } else {
    sendZero();
  }
  // Send stopbit.
  delayMicroseconds(BIT_TIME_US);
  sendOne();
  delayMicroseconds(BIT_TIME_US);
  sendZero();
}


// ___________________________________________________________________
void IRSender::sendOne() {
  TCCR2B = TCCR2B | B00000001;
}

// ___________________________________________________________________
void IRSender::sendZero() {
  TCCR2B = TCCR2B & B11111000;
}

// ___________________________________________________________________
void IRSender::setupTimer() {
  // Setup Timer2 so that there is a PWM-signal with 38kHz and
  // duty-cycle of 50% on pin 3.
  // Set Clear Timer on Compare Match (CTC) mode and toggle the pin
  // each timer the compare-value is reached -> "PWM" with fixed
  // 50% Duty-Cycle.
  TCCR2A = _BV(WGM21) | _BV(COM2B0);
  // Set the compare-value to 210:
  // -> Divide clock at output B by 210 -> 1/2 cycle = 76 kHz
  // -> 1 cycle = 38 kHz
  OCR2A = 210;
}
