// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>

#include <mraa.hpp>

#include <string>

int open_port(std::string port) {
  int fd; // file description for serial port

  // open, ReadWrite + never controlling + non-blocking
  fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd == -1) {
    printf("open_port: Unable to open %s.\n", port.c_str());
  } else {
    fcntl(fd, F_SETFL, 0); // set file status flags
    printf("port %s is open.\n", port.c_str());
  }

  return fd;
}

int configure_port(int fd) {
  // struct to store settings
  struct termios port_settings;

  // baud rates
  cfsetispeed(&port_settings, B38400);
  cfsetospeed(&port_settings, B38400);

  // set parity, stop bits, data bits
  port_settings.c_cflag &= ~PARENB;
  port_settings.c_cflag &= ~CSTOPB;
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;

  // apply settings
  tcsetattr(fd, TCSANOW, &port_settings);
  return fd;
}

int bt_write(int fd, std::string s) {
  write(fd, s.c_str(), s.length());
  return 0;
}

int bt_setup(int fd) {
  bt_write(fd, "\r\n+STWMOD=0\r\n");
  bt_write(fd, "\r\n+STNA=EdisonBTSlave\r\n");
  bt_write(fd, "\r\n+STPIN=0000\r\n");
  bt_write(fd, "\r\n+STOAUT=1\r\n");
  bt_write(fd, "\r\n+STAUTO=0\r\n");
  usleep(2000000);
  bt_write(fd, "\r\n+INQ=1\r\n");
  usleep(2000000);
  printf("Slave running and inquirable.\n");

  return 0;
}

bool bt_available(int fd, int timeout) {
  int retval;
  fd_set rfds;
  struct timeval tv;

  // populate file descriptor set
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);

  // timeout for select
  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  // wait until port is available, until timeout
  retval = select(fd + 1, &rfds, NULL, NULL, &tv);

  if (retval == -1) {
    perror("select() failed\n");
  } else if (retval) {
    printf("Data available.\n");
    return true;
  } else {
    printf("Timeout, no data.\n");
  }

  return false;
}

int main(int argc, char** argv) {
  int bt_serial = open_port("/dev/ttyMFD1");
  configure_port(bt_serial);
  bt_setup(bt_serial);

  close(bt_serial);
  printf("Closed.\n");

  return 0;
}
