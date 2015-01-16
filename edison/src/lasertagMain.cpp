// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <syslog.h>

#include <string>
#include <iostream>
#include <sstream>

#include "./lasertag.h"

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

  lasertag lt(6);
  lt.i2c()->address(0x68);

  lt.dsp_init();
  lt.dsp()->setOrientation(3);

  lt.tcp_init(argv[1]);
  lt.bt_init(argv[2]);

  printf("\n");
  fflush(stdout);

  uint8_t i = 0;
  uint8_t j = 0;

  int code = 0;
  int pos = 0;

  while (running == 1 && lt.bt()->bt_connected()) {
    int i2c_rec = lt.i2c_read_int();

    if (i2c_rec != 0) {
      printf("%d\n", i2c_rec);
      fflush(stdout);
    }

    if (i2c_rec != 0 && i2c_rec != 255) {
      lt.dsp()->drawText(10, ((i++)*lt.dsp()->fontY())+10, std::to_string(i2c_rec));
      if ((i*lt.dsp()->fontY())+10 > lt.dsp()->maxY()-2*lt.dsp()->fontY())
        i = 0;
      lt.tcp()->tcp_send(std::to_string(i2c_rec).append("\n"));
    }

    if (lt.bt()->available(0,1000)) {
      int rec = (uint8_t)lt.bt()->serial_read();
      //printf("%d\n", rec);
      if (rec == 255) {
        std::stringstream ss;
        while (!lt.bt()->available(0,1000));
        code = (uint8_t)lt.bt()->serial_read();
        while (!lt.bt()->available(0,1000));
        pos = (uint8_t)lt.bt()->serial_read();
        ss << code << "\n"; //<< ":" << pos << "\n";
        //printf("%s\n", ss.str().c_str());
        //fflush(stdout);
        lt.tcp()->tcp_send(ss.str());
      }
    }

    if (lt.tcp()->tcp_available(0, 1000) > 0) {
      std::string tmp = lt.tcp()->tcp_read_string("\n");
      std::stringstream dsp;
      dsp << "Hit by " << tmp << " (" << code << ") at position " << pos;
      printf("%s\n", dsp.str().c_str());
      fflush(stdout);
      lt.dsp()->drawText(10+lt.dsp()->fontX()*0, ((j++)*lt.dsp()->fontY())+10, dsp.str());
      if ((j*lt.dsp()->fontY())+10 > lt.dsp()->maxY()-2*lt.dsp()->fontY())
        j = 0;
    }
  }

  return 0;
}


