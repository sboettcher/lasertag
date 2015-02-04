//
//  config.h
//  
//
//  Created by Benjamin Völker on 23/01/15.
//
//

#ifndef _config_h
#define _config_h

// Debugging defines
#define DEBUG_SERIAL_SPEED 115200
//#define DEBUG
#define DISPLAY_ACTIVE
#define SERVER_ACTIVE
#define WEST_ACTIVE
#define RFID_ACTIVE
#define SENDER_ACTIVE
#define RECEIVER_ACTIVE
#define LED_ACTIVE



struct hit {
    uint8_t position;
    uint8_t code;
};

typedef struct hit Hit;


// PINS
#define SHOOT_BUTTON A0
//LED Strip Pin A1     // defined in LED.h
// Next two are the same pin
#define RECEIVE_PIN 2
#define RECEIVE_INTERRUPT 0
#define IR_LED 3
#define SERVER_SERIAL_RX A1
#define SERVER_SERIAL_TX A3
// Display Pins 8-13  // defined in Display.h

// Server defines and Serial
#define SERVER_SERIAL_SPEED 9600

// West defines and Serial
#define WEST_SERIAL_SPEED 38400

// Delays
#define SHOOT_DELAY 100

// Tag Infos
#define HANDLE_TAG_OFFLINE 1
#define RELOAD_TAG 475
#define RELOAD_TAG2 747
#define RELOAD_DELAY 500
#define REFILL_HEALTH_TAG 490
#define REFILL_HEALTH_TAG2 445
#define REFILL_HEALTH_DELAY 500
#define RELOAD_TAG_AMOUNT 10
#define REFILL_HEALTH_TAG_AMOUNT 10

// The Display Orientation
#define DISPLAY_ORIENTATION 0

// Sending-Time for each bit. Whole sending-time = 8.5 * bitTime.
#define BIT_TIME_US 1000

#endif
