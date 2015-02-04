/*
 * Benjamin Völker, University of Freiburg
 * mail: voelkerb@me.com
 */
 
#include <FastLED.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include "config.h"
#include "LED.h"
#include "west.h"
#include "player.h"
#include "sender.h"
#include "rfid.h"
#include "display.h"

// Serial objects
SoftwareSerial serverSerial = SoftwareSerial(SERVER_SERIAL_RX, SERVER_SERIAL_TX);

// Led Object
#ifdef LED_ACTIVE
LED led(HEALTH_AT_START);
#endif

// Display Object
#ifdef DISPLAY_ACTIVE
Display display(DISPLAY_ORIENTATION, FULL_AMMUNITION, HEALTH_AT_START);
#endif

// Server object
#ifdef WEST_ACTIVE
#ifndef DEBUG
West west(&Serial);
#endif
#endif

// RFID object
#ifdef RFID_ACTIVE
RFID rfid;
#endif

// If error somewhere
boolean _error = false;
int _gameOver = -1;
boolean _isConnected = false;
Hit _lastHit = {0,0};

// The Player
Player player(2, "John Doe", CRGB(0, 0, 255));

// The IR Sender
#ifdef SENDER_ACTIVE
IRSender irSender(SHOOT_BUTTON, IR_LED, LASER_PIN);
#endif

volatile boolean _isrHandling = false;
boolean _showHit = false;

void setup() {
  // Start Debug Serial
#ifdef DEBUG
  Serial.begin(DEBUG_SERIAL_SPEED);
#endif

  // Start Server connection 
#ifdef SERVER_ACTIVE
  serverSerial.begin(SERVER_SERIAL_SPEED);
#endif

  // Start RFID connection 
#ifdef RFID_ACTIVE
  rfid.begin();
#endif

#ifdef DEBUG
  Serial.println("Tagger up and running...");
#endif

  // While tagger not connected to Server, show boot pattern on LEDs
#ifdef SERVER_ACTIVE
  // TODO 
  /*
  while(!_isConnected) {
   #ifdef LED_ACTIVE
   led.updateLED(BOOT_PATTERN, player.getHealth());
   #endif
   if (serverSerial.available()) {
     handleServerCommand();
   }
   #ifdef DEBUG
   Serial.println("Waiting for Server Connection...");
   delay(1000);
   #endif
   }*/
#endif

#ifdef DEBUG
  Serial.println("Connection established");
#endif

  // Init the Player
#ifdef SERVER_ACTIVE
  // TODO
  /*
  Player thePlayer(server.getPlayerID(), server.getPlayerName(), server.getTeamColor());
   player = thePlayer;
   */
#endif



  // Store TeamColor in LED and west
#ifdef LED_ACTIVE
  led.setTeamColor(player._teamColor);
#endif



  // Init the Display
#ifdef DISPLAY_ACTIVE
  display.bootUp();
  display.drawPlayerName(player._playerName);
  display.drawPoints(player._points);
  display.updateAmmo(player._ammunition);
  display.updateHealth(player._health);
#ifdef WEST_ACTIVE
  display.updateInfo("Connecting to Vest...");
#endif
#endif

  // Init the Sender and init Button for shooting
#ifdef SENDER_ACTIVE
  irSender.init(player._id, SHOOT_DELAY);
#endif

  // Show current Health
#ifdef LED_ACTIVE
  led.updateLED(CURR_HEALTH_PATTERN, player._health);
#endif

#ifdef DEBUG
  Serial.print("Player: ");
  Serial.print(player._playerName);
  Serial.print(" , ID: ");
  Serial.println(player._id);
  Serial.print("Color: ");
  Serial.print("\tR: ");
  Serial.print(player._teamColor.red);
  Serial.print("\tG: ");
  Serial.print(player._teamColor.green);
  Serial.print("\tB: ");
  Serial.println(player._teamColor.blue);
  Serial.println(" ready");
#endif

  // Start West connection 
#ifdef WEST_ACTIVE
#ifndef DEBUG
  west.begin(WEST_SERIAL_SPEED, player._id);
#endif
#ifdef DISPLAY_ACTIVE
  display.updateInfo("Vest connected!");
#endif
#endif
#ifndef WEST_ACTIVE
#ifdef DISPLAY_ACTIVE
  display.updateInfo("Go get them!");
#endif
#endif

#ifdef WEST_ACTIVE
#ifndef DEBUG
  west.setTeamColor(player._teamColor);
#endif
#endif

  // Init the interrupt for the IR receiver
#ifdef RECEIVER_ACTIVE
  attachInterrupt(RECEIVE_INTERRUPT, irISR, CHANGE);
#endif
}


void loop() {
  int ledPattern = CURR_HEALTH_PATTERN;
#ifdef SERVER_ACTIVE
  if (_error || _gameOver != -1) {
    ledPattern = ERROR_PATTERN;
#ifdef DEBUG
    Serial.println("Error");
#endif
  } 
  else {
    // If a command is send from the Server handle command first
    if (serverSerial.available()) {
      handleServerCommand();
    }
#endif

    // If an RFID tag is recognized handle it
#ifdef RFID_ACTIVE
    if (rfid.available()) {
      handleRFIDTag();
    }
#endif

    // If West got hit maybe handle hit
#ifdef WEST_ACTIVE
#ifndef DEBUG
    if (west.available()) {
      handleWestHit();
    }
#endif
#endif

    // look if player is alive
    if (player._health > 0) {
      // look if player has ammo
      if (player._ammunition > 0) {
        // look if player wants to shoot
#ifdef SENDER_ACTIVE
        int shooted = irSender.shootIfNeeded();
        if (shooted == 1) {
          handleShoot();
        } 
        else if (shooted == 0) {
          ledPattern = SHOOT_PATTERN;
        }
#endif
      // show relaod Pattern
      } 
      else {
        ledPattern = AMMO_PATTERN;
        digitalWrite(LASER_PIN, LOW);
      }
      // show dead Pattern
    } 
    else {
      ledPattern = DEAD_PATTERN;
      digitalWrite(LASER_PIN, LOW);
    }

#ifdef SERVER_ACTIVE
  }
#endif

#ifdef LED_ACTIVE
  if (_showHit) {
    ledPattern = HIT_PATTERN;
    _showHit = false;
  }
  led.updateLED(ledPattern, player._health);
#endif

}

#ifdef WEST_ACTIVE
#ifndef DEBUG
void handleWestHit() {
  /*
  display.updateInfo("Got Hit");
  delay(1000);*/
  handleHit(west.getCode());
}
#endif
#endif

#ifdef RFID_ACTIVE
void handleRFIDTag() {
  // TODO
  long tag = rfid.getTag();
#ifdef DEBUG
  Serial.print("Got Tag ID: ");
  Serial.println(tag);
  //display.updateInfo(String(tag, DEC));
#endif  
  if (HANDLE_TAG_OFFLINE) {
    if (tag == RELOAD_TAG || tag == RELOAD_TAG2) {
      handleReload(RELOAD_TAG_AMOUNT);
#ifdef DEBUG
      Serial.print("Refill ammo");
#endif
      delay(RELOAD_DELAY);
    } 
    else if (tag == REFILL_HEALTH_TAG || tag == REFILL_HEALTH_TAG2) {
      handleHealth(REFILL_HEALTH_TAG_AMOUNT);
#ifdef DEBUG
      Serial.print("Refill health");
#endif
      delay(REFILL_HEALTH_DELAY);
    }
  } 
  else {
#ifdef SERVER_ACTIVE
    serverSerial.print("ri:");
    for (int i = 0; i < rfid.uid.size; i++) {
      serverSerial.write(rfid.uid.uidByte[i]);
    }
    serverSerial.println("");
#endif
  }
}
#endif


void handleShoot() { 
  player.shooted();
#ifdef LED_ACTIVE
  led.updateLED(SHOOT_PATTERN, player._health);
#endif
#ifdef DISPLAY_ACTIVE
  display.updateAmmo(player._ammunition);
#endif
#ifdef DEBUG
  Serial.println("Player shooted");
#endif
#ifdef SERVER_ACTIVE
  serverSerial.print("as:");
  serverSerial.println(player._ammunition);
#endif
}

void handleHealth(int health) { 
  if (player._health != HEALTH_AT_START) {
#ifdef LED_ACTIVE
    led.updateLED(HEALTH_PATTERN, player._health);
#endif
    player.refillHealth(health, true);
#ifdef DISPLAY_ACTIVE
    display.updateHealth(player._health);
    display.updateInfo("Reload Health");
#endif
#ifdef SERVER_ACTIVE
    //TODO server.sendHealth(player._health);
#endif
#ifdef WEST_ACTIVE
  west.isDead(true);
#endif
  }
}

void handleReload(int ammo) {
  if (player._ammunition != FULL_AMMUNITION) {
    player.reload(ammo, true);
#ifdef LED_ACTIVE
    led.updateLED(RELOAD_PATTERN, player._health);
#endif
#ifdef DISPLAY_ACTIVE
    display.updateAmmo(player._ammunition);
    display.updateInfo("Reload Ammo");
#endif
#ifdef SERVER_ACTIVE
    // TODO: server.sendAmmo(player._ammunition);
#endif
  }
}

void handleHit(Hit hit) {
  if (hit.code != player._id) {
    _lastHit = hit;
    if (player.gotHit() == -1) {
#ifdef DISPLAY_ACTIVE
      display.updateHealth(player._health);
      display.updateInfo(hit);
      handleDead();
#endif
    } 
    else {
#ifdef DISPLAY_ACTIVE
      display.updateHealth(player._health);
      display.updateInfo(hit);
#endif
    }
#ifdef SERVER_ACTIVE
    serverSerial.print("hi:");
    serverSerial.print(hit.code);
    serverSerial.print(":");
    serverSerial.println(hit.position);
#endif
#ifdef LED_ACTIVE
    _showHit = true;
    led.updateLED(HIT_PATTERN, player._health);
#endif
  }
}

void handleDead() { 
  #ifdef WEST_ACTIVE
  west.isDead(true);
  #endif
  #ifdef SERVER_ACTIVE
  //server.sendDead();
  #endif
}


#ifdef RECEIVER_ACTIVE
void irISR() {
  if (_isrHandling) return;
  _isrHandling = true;
  #ifdef DEBUG
  Serial.print("\nReceived code: ");
  #endif
  noInterrupts();
  int code = receive();
  EIFR = 0x01;
  #ifdef DEBUG
  Serial.println(code);
  #endif
  if (code != -1) {
    Hit hit = {
      100, code    };
    handleHit(hit);
  }
  interrupts();
  EIFR = 0x01;
  _isrHandling = false;
}
#endif


#ifdef SERVER_ACTIVE
// ___________________________________________________________________
void handleServerCommand() {
  if (serverSerial.available() > 2) {
    char first = serverSerial.read();
    char second = serverSerial.read();
    char doppelPkt = serverSerial.read();
    if (doppelPkt != ':') return;
    switch (first) {
      // Connect
      case 'c':
        if (serverSerial.available() > 2) {
          if (serverSerial.read() == 1) _isConnected = true;
          else _isConnected = false;
          if (serverSerial.read() == ':') {
            player._id = serverSerial.read();
          }
        }
        break;
      case 'n':
        // NAME
        if (second == 'p') {
          /*
          char name[MAX_NAME_LENGTH];
          for (int i = 0; i < MAX_NAME_LENGTH+1; i++) {
            name[i] = '\0';
          }
          int i = 0;
          while(serverSerial.available()) {
            if (i < MAX_NAME_LENGTH) {
              name[i] = serverSerial.read();
              i++;
            } else {
              serverSerial.read();
            }
          }
          char niceName[i];
          for (int j = 0; j < i; j++) {
            niceName[j] = name[j];
          }
          player._playerName = niceName;
          */
          int i = 0;
          while(serverSerial.available()) {
            if (i < MAX_NAME_LENGTH) {
              player._playerName[i] = serverSerial.read();
              i++;
            } else {
              serverSerial.read();
            }
          }
        }
        break;
      case 'g': 
        // special modes of a game
        if (second == 'm') {
          if (serverSerial.available()) player._gameMode = serverSerial.read();
        // start game
        } else if (second == 's') {
          _gameOver = -1;
        // stop game
        } else if (second == 't') {
          _gameOver = 0;
        // pause game
        } else if (second == 'p') {
          _gameOver = 1;
        // special modes of a person
        } else if (second == 'c') {
          if (serverSerial.available()) player._gameSpecial = serverSerial.read();
        } 
        break;
      // Info
      case 'i': 
        // write something on the display
        if (second == 'n') {
          int i = 0;
          while(serverSerial.available()) {
            if (i < MAX_INFO_TEXT) {
              player._infoText[i] = serverSerial.read();
              i++;
            } else {
              serverSerial.read();
            }
          }
          #ifdef DISPLAY_ACTIVE
          display.updateInfo(player._infoText);
          #endif
        }
        break;
      case 't': 
        // Teamcolor
        if (second == 's') {
          if (serverSerial.available() >= 3) {
            player._teamColor.red = serverSerial.read();
            player._teamColor.green = serverSerial.read();
            player._teamColor.blue = serverSerial.read();
            #ifdef LED_ACTIVE
            led.setTeamColor(player._teamColor);
            #endif
          }
        // TAG points
        } else if (second == 'p') {
          if (serverSerial.available() >= 2) {
            boolean increase = serverSerial.read();
            player.gotPoints(serverSerial.read(), increase);
            #ifdef DISPLAY_ACTIVE
            display.drawPoints(player._points);
            #endif
          }
        }
        break;
      case 'h':
        // health wert
        if (second == 's') {
          if (serverSerial.available()) {
            player._health = serverSerial.read();
            #ifdef DISPLAY_ACTIVE
            display.updateHealth(player._health);
            #endif
          }
        // shooter name
        } else if (second == 'p') {
          int i = 0;
          while(serverSerial.available()) {
            if (i < MAX_NAME_LENGTH) {
              player._otherPlayerName[i] = serverSerial.read();
              i++;
            } else {
              serverSerial.read();
            }
          }
          #ifdef DISPLAY_ACTIVE
          display.updateInfo(player._otherPlayerName, _lastHit.position);
          #endif
        // hit name
        } else if (second == 'v') {
          int i = 0;
          while(serverSerial.available()) {
            if (i < MAX_NAME_LENGTH) {
              player._otherPlayerName[i] = serverSerial.read();
              i++;
            } else {
              serverSerial.read();
            }
          }
          #ifdef DISPLAY_ACTIVE
          display.updateInfo(player._otherPlayerName);
          #endif
        }
        break; 
      // Points
      case 'p':
        if (second == 's') {
          if (serverSerial.available()) {
            player._points = serverSerial.read();
            #ifdef DISPLAY_ACTIVE
            display.drawPoints(player._points);
            #endif
          }
        }
        break;
      // Ammo
      case 'a':
        if (serverSerial.available()) {
          // ammo
          if (second == 's') {
            player._ammunition = serverSerial.read();
            #ifdef DISPLAY_ACTIVE
            display.updateAmmo(player._ammunition);
            #endif
          } else if(second == 'p') {
            player._ammoDecrease = serverSerial.read();
            #ifdef DISPLAY_ACTIVE
            display.updateAmmo(player._ammunition);
            #endif
          } else if(second == 't') {
            player._points = serverSerial.read();
          }
        }
        break;
      case 'r': 
        if (serverSerial.available() > 2) {
          // ammo tag
          if (second == 'a') { 
            bool increment = serverSerial.read();
            player.reload(serverSerial.read(), increment);
          // health tag 
          } else if (second == 'h') {
            bool increment = serverSerial.read();
            player.refillHealth(serverSerial.read(), increment);
          }
        }
        break;
      case 'e': 
        _error = true;
        break;
      }
  }
}
#endif

#ifdef RECEIVER_ACTIVE
// ___________________________________________________________________
int receive() {
  // Search for a rising edge.
  int Code = -1;
  if (digitalRead(RECEIVE_PIN) == LOW) {
    byte c = 0;
    int highcount = 0;
    // Check the pin-input every bitTime µs and save the value.
    for (byte mask = 10000000; mask>0; mask >>= 1) {
      delayMicroseconds(BIT_TIME_US);
      if (!digitalRead(RECEIVE_PIN) == HIGH) {
        c = c | mask;
        highcount++;
      }
    }
    // Read the parity-bit and check for even parity.
    delayMicroseconds(BIT_TIME_US);
    if (digitalRead(RECEIVE_PIN) == LOW) {
      highcount++;
    }
    delayMicroseconds(BIT_TIME_US);
    if (digitalRead(RECEIVE_PIN) == LOW) {
      // If the parity-check was successfull print out the code, else print out an error.
      if (highcount % 2 == 0) {
        Code = (int) c;
      }
    }
    delayMicroseconds(BIT_TIME_US);
  }
  return Code;
}
#endif


