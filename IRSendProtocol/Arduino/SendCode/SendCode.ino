#define TXPin 3      // Pin for the sending-LED.
#define bitTime 10000    // Sending-Time for each bit. Whole sending-time = 8.5 * bitTime.

#define BUTTON_PIN  2
boolean old_button = false;

// ToDo.
void setupTimer() {
  // Setup Timer2 so that there is a PWM-signal with 38kHz and 
  // duty-cycle of 50% on pin 3.
  TCCR2A = _BV(WGM21) | _BV(COM2B0); // Set Clear Timer on Compare Match (CTC) mode and toggle the pin each timer the compare-value is reached -> "PWM" with fixed 50% Duty-Cycle.
  // Set the compare-value to 210:
  OCR2A = 210; // -> Divide clock at output B by 210 -> 1/2 cycle = 76 kHz -> 1 cycle = 38 kHz
}

void startSending() {
  // Activate the clock.
  // TCCR2B = _BV(CS20);  // No clock-divider -> 16MHz
  TCCR2B = TCCR2B | B00000001;  // No clock-divider -> 16MHz
}

void stopSending() {
  // Deactivate the clock.
  TCCR2B = TCCR2B & B11111000;
}

// ToDo.
void sendCode(int Code){
  // Start transmitting with an rising edge.
  startSending();
  delayMicroseconds(bitTime/2);
  // Convert the code into a byte.
  byte c = byte(Code & 0x00FF);
  int highCount = 0;
  // Check for each bit in the byte if it is a 1 or a 0 and send a
  // signal correspondingly for a given time.
  for (byte mask = 1; mask>0; mask <<= 1) {
    if (c & mask) {
      startSending();
      highCount++;  // Count the 1s for parity
      Serial.println("H");
    } else {
      stopSending();
      Serial.println("L");
    }
    delayMicroseconds(bitTime);
  }
  // Add a parity-bit.
  if (highCount % 2 == 1) {
    startSending();
  } else {
    stopSending();
  }
  // Send stopbit.
  delayMicroseconds(bitTime);
  startSending();
  delayMicroseconds(bitTime);
  stopSending();
}

// Setup-routine.
void setup(){
  // Start serial-communication.
  Serial.begin(57600);
  // Set pin-modes.
  pinMode(TXPin, OUTPUT); 
  
  pinMode(BUTTON_PIN, INPUT);
  
  // SetupTimer.
  setupTimer();
  stopSending();
  delay(1000);
}

// Main-loop.
void loop(){
  if (!digitalRead(BUTTON_PIN) && !old_button) {
    sendCode(1);
    digitalWrite(13, HIGH);
    old_button = true;
  }
  if (digitalRead(BUTTON_PIN)) {
    old_button = false;
    digitalWrite(13, LOW);
  }
}
