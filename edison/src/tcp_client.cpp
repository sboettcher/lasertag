// Copyright 2014 Sebastian Boettcher

#include <string>

#include "./tcp_client.h"

tcp_client::tcp_client() {
  m_connected = false;
}

tcp_client::~tcp_client() {
  close(m_socketFD);
}

void tcp_client::error_exit(std::string errorMessage) {
  fprintf(stderr, "%s: %s\n", errorMessage.c_str(), strerror(errno));
  exit(EXIT_FAILURE);
}

void tcp_client::tcp_connect(std::string ip) {
  // create socket
  m_socketFD = socket(AF_INET, SOCK_STREAM, 0);
  if (m_socketFD < 0)
    error_exit("Error creating socket.");

  struct sockaddr_in server;
  struct hostent *host_info;
  unsigned long addr;
  
  // get server ip, if not valid try to get via server name
  memset(&server, 0, sizeof(server));
  if((addr = inet_addr(ip.c_str())) != INADDR_NONE) {
    memcpy((char *)&server.sin_addr, &addr, sizeof(addr));
  } else {
    host_info = gethostbyname(ip.c_str());
    if (NULL == host_info)
      error_exit("Unknown server.");
     memcpy((char *)&server.sin_addr, host_info->h_addr, host_info->h_length);
  }

  // set to IPv4 and port
  server.sin_family = AF_INET;
  server.sin_port = htons(PORT);

  // connect to server
  printf("Connecting to server @ %s\n", ip.c_str());
  fflush(stdout);
  if(connect(m_socketFD, (struct sockaddr*)&server, sizeof(server)) < 0)
    error_exit("Could not connect to server!");
  printf("Connected!\n");
  fflush(stdout);

  m_connected = true;
}

void tcp_client::tcp_send(std::string s) {
  if (!m_connected)
    return;

  if(send(m_socketFD, s.c_str(), s.size(), 0) != (ssize_t)s.size())
    error_exit("Wrong number of bytes sent!");
}

std::string tcp_client::tcp_read() {
  if (!m_connected)
    return "";

  int rec_len;
  char buf[RCVBUFSIZE];
  memset(buf, 0, RCVBUFSIZE); // clear buffer
  rec_len = read(m_socketFD, buf, RCVBUFSIZE);
  if(rec_len < 0){
    perror("error reading stream message");
    exit(1);
  } else {
    return std::string(buf);
  }
}

std::string tcp_client::tcp_read_single() {
  if (!m_connected)
    return "";

  int rec_len;
  char buf[2]; // 1 char + null terminate
  memset(buf, 0, 2); // clear buffer
  rec_len = read(m_socketFD, buf, 1);
  if(rec_len < 0){
    perror("error reading stream message");
    exit(1);
  } else {
    //write(1, buf, 2);
    //fflush(stdout);
    return std::string(buf);
  }
}

std::string tcp_client::tcp_read_string(std::string term) {
  if (!m_connected)
    return "";

  std::string ret;
  std::string tmp;
  while (tmp != term) {
    while (!tcp_available(0,1000)) {
      // do nothing, wait for available
    }
    ret.append(tmp);
    tmp = tcp_read_single();
  }
  return ret;
}

int tcp_client::tcp_available(int sec, int usec) {
  if (!m_connected)
    return -1;

  int retval;
  fd_set rfds;
  struct timeval tv;

  // populate file descriptor set
  FD_ZERO(&rfds);
  FD_SET(m_socketFD, &rfds);

  // timeout for select
  tv.tv_sec = sec;
  tv.tv_usec = usec;

  // wait until port is available, until timeout
  retval = select(m_socketFD + 1, &rfds, NULL, NULL, &tv);

  return retval;
}


