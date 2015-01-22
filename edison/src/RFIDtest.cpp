// Copyright 2015 Sebastian Boettcher

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include <string>

#include "./MFRC522.h"

#define RFID_RST_PIN 32
#define RFID_CS_PIN 33

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    printf("\nclosing nicely\n");
    running = 0;
  }
}

int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  printf("Constructor... ");
  fflush(stdout);
  MFRC522 rfid(RFID_RST_PIN, RFID_CS_PIN);
  printf("Done.\n");
  fflush(stdout);

  while (running == 1) {
    if (!rfid.PICC_IsNewCardPresent())
      continue;
    if (!rfid.PICC_ReadCardSerial())
      continue;
    rfid.printCardID();
  }
  
  return 0;
}
