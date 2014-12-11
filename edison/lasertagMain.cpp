// Copyright 2014 Sebastian Boettcher

#include <signal.h>
#include <syslog.h>

#include "./lasertag.h"

int running = 1;

void sig_handler(int signo) {
  if (signo == SIGINT) {
    running = 0;
  }
}


int main(int argc, char** argv) {
  signal(SIGINT, sig_handler);

  lasertag lt;
  lt.init();
  lt.active(true);

  lt.i2c()->address(0x68);

  uint8_t i = 2;

  while (running == 1) {
    int rec = lt.i2c()->read();
    if (rec != 0) {
      printf("%d\n", rec);
      fflush(stdout);

      lt.lcd_write_int(rec, i++, 0);
      if (i > 10)
        i = 2;
    }
    usleep(1000);
  }

  return 0;
}
