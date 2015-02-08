// Copyright 2014 Sebastian Boettcher

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <string>

#define PORT 2000
#define RCVBUFSIZE 8192

class tcp_client {
  public:
    tcp_client();
    ~tcp_client();

    void error_exit(std::string errorMessage);

    // connect to server with ip or host name
    void tcp_connect(std::string ip);

    // send a string to the server
    void tcp_send(std::string s);

    // read RCVBUFSIZE chars
    std::string tcp_read();
    // read one char
    std::string tcp_read_single();
    // read until [term] occurs
    std::string tcp_read_string(std::string term);

    // >0 if data is available, 0 if timeout, -1 if error
    int tcp_available(int sec, int usec);

    bool connected() {
      return m_connected;
    }
    
  private:
    int m_socketFD;
    bool m_connected;
};

