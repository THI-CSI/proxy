#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <vector>

class Proxy{
private:
  int server_fd;
  struct sockaddr_in address;
  int address_len = sizeof(address);
  char buffer[1024];
  std::string dir;

  std::string getHeader(std::string buffer, std::string newHost);
  std::string getRequestPath(std::string buffer);
  std::string getHost(std::string buffer);
  int checkHost(std::string* host);
  std::string loadFile(std::string path);
  std::vector<std::string> customSplit(std::string str, char separator);
  void forwardRequestToHost(const std::string& host, int clientSocket, const std::string& request);
  int createHostSocket(const std::string& host);

public:
  Proxy(int port, std::string directory);
  void init();
  ~Proxy();
};


