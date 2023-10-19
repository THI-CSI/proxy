#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>

class Webserver{
private:
  int server_fd;
  struct sockaddr_in address;
  int address_len = sizeof(address);
  char buffer[1024];
public:
  Webserver(int port, std::string directory);
  void init();
  ~Webserver();
};


