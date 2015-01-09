// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <syslog.h>

#include <string>

#include "./lasertag.h"
#include "./tcp_client.h"

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}


int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  tcp_client client;
  client.tcp_connect("192.168.0.104");

  lasertag lt(6);
  lt.i2c()->address(0x68);
  lt.init_ILI9225();
  lt.ILI9225()->setOrientation(3);

  uint8_t i = 0;
  uint8_t j = 0;

  while (running == 1) {
    int rec = lt.i2c()->read();

    if (rec != 0) {
      printf("%d\n", rec);
      fflush(stdout);
    }

    if (rec != 0 && rec != 255) {
      lt.ILI9225()->drawText(10, ((i++)*lt.ILI9225()->fontY())+10, std::to_string(rec));
      if ((i*lt.ILI9225()->fontY())+10 > lt.ILI9225()->maxY()-2*lt.ILI9225()->fontY())
        i = 0;
      client.tcp_send(std::to_string(rec).append("\n"));
    }


    if (client.tcp_available(0, 1000) > 0) {
      std::string tmp = client.tcp_read_string("\n");
      printf("%s", tmp.c_str());
      fflush(stdout);
      lt.ILI9225()->drawText(30, ((j++)*lt.ILI9225()->fontY())+10, tmp);
      if ((j*lt.ILI9225()->fontY())+10 > lt.ILI9225()->maxY()-2*lt.ILI9225()->fontY())
        j = 0;
    }
  }

  return 0;
}
