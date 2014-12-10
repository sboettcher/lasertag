// Copyright 2014 Sebastian Boettcher

#include <string>
#include "./bt_serial.h"


bt_serial::bt_serial(std::string port)
  : m_port("/dev/ttyMFD1"), m_fd(0) {
  m_port = port;
  if (open_port()) {
    configure_port();
  }
}

bt_serial::~bt_serial() {
  close(m_fd);
  printf("Closed port %s.\n", m_port.c_str());
}

bool bt_serial::open_port() {
  // open, ReadWrite + never controlling + non-blocking
  m_fd = open(m_port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

  if (m_fd == -1) {
    printf("open_port: Unable to open %s.\n", m_port.c_str());
    return false;
  } else {
    fcntl(m_fd, F_SETFL, 0); // set file status flags
    printf("Port %s is open.\n", m_port.c_str());
  }

  return true;
}

void bt_serial::configure_port() {
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
  tcsetattr(m_fd, TCSANOW, &port_settings);

  printf("Port %s is configured.\n", m_port.c_str());
}


void bt_serial::init() {
  bt_write("\r\n+STWMOD=0\r\n");
  bt_write("\r\n+STNA=EdisonBTSlave\r\n");
  bt_write("\r\n+STPIN=0000\r\n");
  bt_write("\r\n+STOAUT=1\r\n");
  bt_write("\r\n+STAUTO=0\r\n");
  usleep(2000000);
  bt_write("\r\n+INQ=1\r\n");
  usleep(2000000);
  printf("Slave running and inquirable.\n");
}


bool bt_serial::available(int timeout) {
  int retval;
  fd_set rfds;
  struct timeval tv;

  // populate file descriptor set
  FD_ZERO(&rfds);
  FD_SET(m_fd, &rfds);

  // timeout for select
  tv.tv_sec = timeout;
  tv.tv_usec = 0;

  // wait until port is available, until timeout
  retval = select(m_fd + 1, &rfds, NULL, NULL, &tv);

  if (retval == -1) {
    perror("select() failed\n");
  } else if (retval) {
    // printf("Data available.\n");
    return true;
  } else {
    printf("Timeout, no data.\n");
  }

  return false;
}


char bt_serial::bt_read() {
  char c;
  read(m_fd, &c, 1);
  return c;
}

void bt_serial::bt_write(std::string s) {
  write(m_fd, s.c_str(), s.length());
}


int bt_serial::get_serial_fd() {
  return m_fd;
}

