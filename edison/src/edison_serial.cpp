// Copyright 2014 Sebastian Boettcher

#include <string>
#include "./edison_serial.h"


edison_serial::edison_serial(std::string port)
  : m_port("/dev/ttyMFD1"), m_fd(0), m_port_ready(false),
    m_bt_connected(false)
{
  m_port = port;
  if (open_port()) {
    configure_port();
    m_port_ready = true;
  }
}

edison_serial::~edison_serial() {
  close(m_fd);
  printf("Closed port %s.\n", m_port.c_str());
}

bool edison_serial::open_port() {
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

void edison_serial::configure_port() {
  // struct to store settings
  struct termios port_settings;

  // baud rates
  cfsetispeed(&port_settings, B38400);
  cfsetospeed(&port_settings, B38400);

  // no parity, one stop bit, 8 data bits
  port_settings.c_cflag &= ~PARENB;
  port_settings.c_cflag &= ~CSTOPB;
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;

  // apply settings
  tcsetattr(m_fd, TCSANOW, &port_settings);

  printf("Port %s is configured.\n", m_port.c_str());
}


void edison_serial::bt_slave_init(std::string name) {
  if (!m_port_ready) {
    printf("Serial port not ready.\n");
    return;
  }

  serial_write("\r\n+STWMOD=0\r\n");
  serial_write("\r\n+STNA=" + name + "\r\n");
  serial_write("\r\n+STPIN=0000\r\n");
  serial_write("\r\n+STOAUT=1\r\n");
  serial_write("\r\n+STAUTO=0\r\n");
  usleep(2000000);
  serial_write("\r\n+INQ=1\r\n");
  usleep(2000000);
  printf("Slave running and inquirable.\n");

  m_bt_connected = true;
}

void edison_serial::bt_master_init(std::string name, std::string slave) {
  if (!m_port_ready) {
    printf("Serial port not ready.\n");
    return;
  }

  serial_write("\r\n+STWMOD=1\r\n");
  serial_write("\r\n+STNA=" + name + "\r\n");
  serial_write("\r\n+STPIN=0000\r\n");
  serial_write("\r\n+STAUTO=0\r\n");
  usleep(2000000);
  serial_write("\r\n+INQ=1\r\n");
  usleep(2000000);
  printf("Master running and inquiring for slave %s.\n", slave.c_str());

  // find target slave
  std::string buf;
  std::string slaveAddr;
  int nameIndex = 0;
  int addrIndex = 0;

  while (true) {
    if (available(1)) {
      buf += serial_read();
      nameIndex = buf.find(";" + slave);
      if (nameIndex != -1) {
        addrIndex = buf.find("+RTINQ=") + 7;
        slaveAddr = buf.substr(addrIndex, nameIndex - addrIndex);
        break;
      }
    }
  }

  // connect to slave
  m_bt_connected = false;
  buf = "";
  while (!m_bt_connected) {
    printf("Connecting to slave: %s @ %s\n", slave.c_str(), slaveAddr.c_str());
    serial_write("\r\n+CONN=" + slaveAddr + "\r\n");
    while (true) {
      if (available(1)) {
        buf += serial_read();
        if (buf.find("CONNECT:OK") != std::string::npos) {
          m_bt_connected = true;
          printf("Connected!\n");
          break;
        } else if (buf.find("CONNECT:FAIL") != std::string::npos) {
          printf("Connect failed, trying again.\n");
          break;
        } else if (buf.find("ERROR") != std::string::npos) {
          printf("ERROR while trying to connect, aborting.\n");
          return;
        }
      }
    }
  }
}


bool edison_serial::available(int timeout) {
  if (!m_port_ready) {
    printf("Serial port not ready.\n");
    return false;
  }

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
    // printf("Timeout, no data.\n");
  }

  return false;
}


char edison_serial::serial_read() {
  if (!m_port_ready) {
    printf("Serial port not ready.\n");
    return 0;
  }

  char c;
  read(m_fd, &c, 1);
  //printf("%c", c);
  //fflush(stdout);
  return c;
}

void edison_serial::serial_write(std::string s) {
  if (!m_port_ready) {
    printf("Serial port not ready.\n");
    return;
  }

  write(m_fd, s.c_str(), s.length());
}



