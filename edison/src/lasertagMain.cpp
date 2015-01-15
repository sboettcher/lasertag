// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <syslog.h>

#include <string>
#include <iostream>
#include <sstream>

#include "./lasertag.h"
#include "./tcp_client.h"
#include "./edison_serial.h"

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}


int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  if (argc != 3) {
    printf("usage: lasertagMain [server host] [BT slave]\n");
    exit(1);
  }

  edison_serial bluetooth("/dev/ttyMFD1");
  bluetooth.bt_master_init("EdisonBTMaster01", argv[2]);

  return 0;
  tcp_client client;
  client.tcp_connect(argv[1]);

  lasertag lt(6);
  lt.i2c()->address(0x68);
  lt.init_ILI9225();
  lt.ILI9225()->setOrientation(3);

  printf("\n");
  fflush(stdout);

  uint8_t i = 0;
  uint8_t j = 0;

  int code = 0;
  int pos = 0;

  while (running == 1 && bluetooth.bt_connected()) {
    int i2c_rec = lt.i2c_read_int();

    if (i2c_rec != 0) {
      printf("%d\n", i2c_rec);
      fflush(stdout);
    }

    if (i2c_rec != 0 && i2c_rec != 255) {
      lt.ILI9225()->drawText(10, ((i++)*lt.ILI9225()->fontY())+10, std::to_string(i2c_rec));
      if ((i*lt.ILI9225()->fontY())+10 > lt.ILI9225()->maxY()-2*lt.ILI9225()->fontY())
        i = 0;
      client.tcp_send(std::to_string(i2c_rec).append("\n"));
    }

    if (bluetooth.available(0,1000)) {
      int rec = (uint8_t)bluetooth.serial_read();
      //printf("%d\n", rec);
      if (rec == 255) {
        std::stringstream ss;
        while (!bluetooth.available(0,1000));
        code = (uint8_t)bluetooth.serial_read();
        while (!bluetooth.available(0,1000));
        pos = (uint8_t)bluetooth.serial_read();
        ss << code << "\n"; //<< ":" << pos << "\n";
        //printf("%s\n", ss.str().c_str());
        //fflush(stdout);
        client.tcp_send(ss.str());
      }
    }

    if (client.tcp_available(0, 1000) > 0) {
      std::string tmp = client.tcp_read_string("\n");
      std::stringstream dsp;
      dsp << "Hit by " << tmp << " (" << code << ") at position " << pos;
      printf("%s\n", dsp.str().c_str());
      fflush(stdout);
      lt.ILI9225()->drawText(10+lt.ILI9225()->fontX()*0, ((j++)*lt.ILI9225()->fontY())+10, dsp.str());
      if ((j*lt.ILI9225()->fontY())+10 > lt.ILI9225()->maxY()-2*lt.ILI9225()->fontY())
        j = 0;
    }
  }

  return 0;
}
