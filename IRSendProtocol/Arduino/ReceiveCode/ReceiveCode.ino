#define RXPin 2          // Receiver-Pin.
#define bitTime 1000    // Sending-Time for each bit in us.

void setup() {
  Serial.begin(57600);
  pinMode(RXPin, INPUT);
}

void loop() {
  // Search for a rising edge.
  if (digitalRead(RXPin) == LOW) {
    byte c = 0;
    int highcount = 0;
    // Check the pin-input every bitTime µs and save the value.
    for (byte mask = 0000001; mask>0; mask <<= 1) {
      delayMicroseconds(bitTime);
      if (!digitalRead(RXPin) == HIGH) {
          c = c | mask;
          highcount++;
          Serial.println("h");
      } else {
          Serial.println("l");
      }
    }
    // Read the parity-bit and check for even parity.
    delayMicroseconds(bitTime);
    if (!digitalRead(RXPin) == HIGH) {
        highcount++;
    }
    delayMicroseconds(bitTime);
    if (!digitalRead(RXPin) == HIGH) {
      // If the parity-check was successfull print out the code, else print out an error.
      if (highcount % 2 == 0) {
          int Code = (int) c;
          Serial.print("Received Code: ");
          Serial.println(c);
          
      } else {
        Serial.println("Parity-Error!");
      }
    } else {
      Serial.println("Stopbit-Error!");
    }
    delayMicroseconds(bitTime);
  }
}
