// Copyright 2014 Sebastian Boettcher

#include <string>
#include <stdexcept>
#include "./edison_serial.h"


edison_serial::edison_serial(std::string port, speed_t baud)
  : m_port("/dev/ttyMFD1"), m_fd(0), m_port_ready(false),
    m_bt_connected(false)
{
  m_port = port;
  if (open_port()) {
    configure_port(baud);
    m_port_ready = true;
  }
}

edison_serial::~edison_serial() {
  close(m_fd);
  printf("[SERIAL] Closed port %s.\n", m_port.c_str());
}

bool edison_serial::open_port() {
  // open, ReadWrite + never controlling + non-blocking
  m_fd = open(m_port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);

  if (m_fd == -1) {
    printf("[SERIAL] open_port: Unable to open %s.\n", m_port.c_str());
    return false;
  } else {
    fcntl(m_fd, F_SETFL, 0); // set file status flags
    printf("[SERIAL] Port %s is open.\n", m_port.c_str());
  }

  return true;
}

void edison_serial::configure_port(speed_t baud) {
  // struct to store settings
  struct termios port_settings;

  // baud rates
  cfsetispeed(&port_settings, baud);
  cfsetospeed(&port_settings, baud);

  // no parity, one stop bit, 8 data bits
  port_settings.c_cflag &= ~PARENB;
  port_settings.c_cflag &= ~CSTOPB;
  port_settings.c_cflag &= ~CSIZE;
  port_settings.c_cflag |= CS8;

  // apply settings
  tcsetattr(m_fd, TCSANOW, &port_settings);

  printf("[SERIAL] Port %s is configured.\n", m_port.c_str());
}


void edison_serial::bt_slave_init(std::string name) {
  if (!m_port_ready) {
    printf("[BLUETOOTH] Serial port not ready.\n");
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
  printf("[BLUETOOTH] Slave running and inquirable.\n");

  m_bt_connected = true;
}

void edison_serial::bt_master_init(std::string name, std::string slave) {
  if (!m_port_ready) {
    printf("[BLUETOOTH] Serial port not ready.\n");
    return;
  }

  serial_write("\r\n+STWMOD=1\r\n");
  serial_write("\r\n+STNA=" + name + "\r\n");
  serial_write("\r\n+STPIN=0000\r\n");
  serial_write("\r\n+STAUTO=0\r\n");
  usleep(2000000);
  serial_write("\r\n+INQ=1\r\n");
  usleep(2000000);
  printf("[BLUETOOTH] Master running and inquiring for slave %s.\n", slave.c_str());

  // find target slave
  std::string buf;
  std::string slaveAddr;
  int nameIndex = 0;
  int addrIndex = 0;

  while (true) {
    if (available(1)) {
      buf += serial_read();

      // limit size of buffer slave name size + offset,
      // this prevents parsing the wrong address
      if (buf.size() > 30 + slave.size())
        buf = buf.substr(1);

      //printf("%s\n", buf.c_str());
      //fflush(stdout);

      nameIndex = buf.find(";" + slave);
      if (nameIndex != -1) {
        addrIndex = buf.find("+rtinq=") + 7;
        slaveAddr = buf.substr(addrIndex, nameIndex - addrIndex);
        break;
      }
    }
  }

  // connect to slave
  m_bt_connected = false;
  buf = "";
  while (!m_bt_connected) {
    printf("[BLUETOOTH] Connecting to slave: %s @ %s\n", slave.c_str(), slaveAddr.c_str());
    serial_write("\r\n+conn=" + slaveAddr + "\r\n");
    while (true) {
      if (available(1)) {
        buf += serial_read();
        if (buf.find("connect:ok") != std::string::npos) {
          m_bt_connected = true;
          printf("[BLUETOOTH] Connected!\n");
          break;
        } else if (buf.find("connect:fail") != std::string::npos) {
          printf("[BLUETOOTH] Connect failed, trying again.\n");
          break;
        } else if (buf.find("error") != std::string::npos) {
          printf("[BLUETOOTH] ERROR while trying to connect, aborting.\n");
          return;
        }
      }
    }
  }
}


bool edison_serial::available(int sec, int usec) {
  if (!m_port_ready) {
    printf("[SERIAL] Serial port not ready.\n");
    return false;
  }

  int retval;
  fd_set rfds;
  struct timeval tv;

  // populate file descriptor set
  FD_ZERO(&rfds);
  FD_SET(m_fd, &rfds);

  // timeout for select
  tv.tv_sec = sec;
  tv.tv_usec = usec;

  // wait until port is available, until timeout
  retval = select(m_fd + 1, &rfds, NULL, NULL, &tv);

  if (retval == -1) {
    perror("[SERIAL] select() failed\n");
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
    printf("[SERIAL] Serial port not ready.\n");
    return 0;
  }

  char c;
  if (read(m_fd, &c, 1) == -1) {
    printf("[SERIAL] read() error: %s\n", strerror(errno));
    fflush(stdout);
  }
  //if (c == 13) {
  //  c = ' ';
  //}
  //printf("%d\n", c);
  //fflush(stdout);
  return (char) std::tolower(c);
}

void edison_serial::serial_write(std::string s) {
  if (!m_port_ready) {
    printf("[SERIAL] Serial port not ready.\n");
    return;
  }

  if (write(m_fd, s.c_str(), s.length()) == -1) {
    printf("[SERIAL] write() error: %s\n", strerror(errno));
    fflush(stdout);
  }
}

void edison_serial::serial_write(char c) {
  if (!m_port_ready) {
    printf("[SERIAL] Serial port not ready.\n");
    return;
  }

  if (write(m_fd, &c, 1) == -1) {
    printf("[SERIAL] write() error: %s\n", strerror(errno));
    fflush(stdout);
  }
}



