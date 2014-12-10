// Copyright 2014 Sebastian Boettcher

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>


class bt_serial {
  public:
    // constructor, takes the serial port (/dev/...)
    bt_serial(std::string port);
    ~bt_serial();

    void bt_setup();
    bool bt_available(int timeout);
    char bt_read();
    void bt_write(std::string s);

  private:
    bool open_port();
    void configure_port();

    std::string m_port;
    int m_fd; // file description for serial port
};
