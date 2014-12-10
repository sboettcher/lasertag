// Copyright 2014 Sebastian Boettcher

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>


class edison_serial {
  public:
    // constructor, takes the serial port (/dev/...)
    edison_serial(std::string port);
    ~edison_serial();

    // send bt slave initiation
    void bt_slave_init(std::string name);
    // send bt master initiation and inquire for given slave
    void bt_master_init(std::string name, std::string slave);

    // wait for available data on port, false if timeout (sec) or error
    bool available(int timeout);
    // read one byte of data and return it
    char serial_read();
    // write the given string to port
    void serial_write(std::string s);

    int get_serial_fd();

  private:
    // open and configure given port
    bool open_port();
    void configure_port();

    std::string m_port;
    int m_fd; // file description for serial port
    bool m_port_ready;
};
