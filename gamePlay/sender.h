#ifndef SENDER_H
#define SENDER_H
#include "inttypes.h"
#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// Sending-Time for each bit. Whole sending-time = 8.5 * bitTime.
#define BIT_TIME_US 1000
#define MASK 10000000

class IRSender {
public:
  
  // Constructor
  IRSender(uint8_t buttonPin, uint8_t ledPin);
  
  // init the Interrupt to the sending pin
  void init(uint8_t code, int shootDelay);
  
  // Look if need to shoot, return if did
  int shootIfNeeded();
  
  // Sends a code
  void send(uint8_t code);
  
private:
  void setupTimer();
  
  void sendOne();
  
  void sendZero();
  
  // Pin for the sending-LED.
  uint8_t _irLEDPin;
  // Button Pin
  uint8_t _buttonPin;
  // Code to send over IR
  uint8_t _code;
  // Delay between two shots
  int _shootDelay;
  
  // Timer
  long _sendTimer;
};

#endif
