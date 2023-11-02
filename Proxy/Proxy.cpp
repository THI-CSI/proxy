#include "Proxy.h"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <fstream>
#include <netdb.h>
#include <cstring>

Proxy::Proxy(int port, std::string directory){
  dir = directory;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if(server_fd < 0){
    std::cerr << "[-] Socket failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  int socket_binding = bind(server_fd, (struct sockaddr*)&address, sizeof(address));
  if(socket_binding < 0){
    std::cerr << "[-] Bind connection failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  
}

void Proxy::init(){
  int new_socket, socket_listener, buffer_len;
  while(true){
    socket_listener = listen(server_fd, 3);
    if(socket_listener < 0){
      std::cerr << "[-] Listen to port failed" << std::endl;
      exit(EXIT_FAILURE);
    }
    
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&address_len);
    if(new_socket <0){
      std::cerr << "[-] Could not accept the connection" << std::endl;
      exit(EXIT_FAILURE);
    }

    buffer_len = read(new_socket, buffer, 1024);
    if(buffer_len == -1){
      std::cerr << "[-] Could not read the request buffer" << std::endl;
      exit(EXIT_FAILURE);
    }
    
    // std::string request_path = getRequestPath(buffer);
    std::string host = getHost(buffer);
    std::string oldHost = host;
    std::string payload = buffer;
    switch(checkHost(&host)) {
      case 0:
        printf("[Host: %s] Allowed.\n", oldHost.c_str());
        break;
      case 1: {
          printf("[Host: %s] Blacklisted.\n", oldHost.c_str());
          std::string forbiddenResponse = "HTTP/1.1 403 FORBIDDEN\r\nContent-Length: 30\r\n\r\nRequested Host is blacklisted!\r\n\r\n";
          send(new_socket, forbiddenResponse.c_str(), forbiddenResponse.length(), 0);
          close(new_socket);
        }
        break;
      case 2:
        printf("[Host: %s] Redirected to %s.\n", oldHost.c_str(), host.c_str());
        payload = getHeader(buffer, host);
        break;
    }

    forwardRequestToHost(host, new_socket, payload);
    close(new_socket);
  }
}

void Proxy::forwardRequestToHost(const std::string& host, int clientSocket, const std::string& request) {
  // Create a socket for the host connection
  int hostSocket = createHostSocket(host);
  if (hostSocket < 0) {
    // Handle the error and return an appropriate response to the client
    std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
    send(clientSocket, errorResponse.c_str(), errorResponse.length(), 0);
    close(clientSocket);
    return;
  }

  // Forward the client's request to the host
  send(hostSocket, request.c_str(), request.length(), 0);

  // Read the host's response
  char responseBuffer[1024];
  int bytesRead;
  while ((bytesRead = recv(hostSocket, responseBuffer, sizeof(responseBuffer), 0)) > 0) {
    send(clientSocket, responseBuffer, bytesRead, 0);
  }

  // Close both sockets
  close(hostSocket);
}

int Proxy::createHostSocket(const std::string& host) {
  // Resolve the host to an IP address
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if (getaddrinfo(host.c_str(), "80", &hints, &res) != 0) {
    return -1;
  }

  int hostSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  if (hostSocket < 0) {
    freeaddrinfo(res);
    return -1;
  }

  if (connect(hostSocket, res->ai_addr, res->ai_addrlen) < 0) {
    freeaddrinfo(res);
    close(hostSocket);
    return -1;
  }

  freeaddrinfo(res);
  return hostSocket;
}


std::string Proxy::getHeader(std::string buffer, std::string newHost) {
  size_t hostPosition = buffer.find("Host:");

  if (hostPosition != std::string::npos) {
    size_t endOfHostLine = buffer.find("\r\n", hostPosition);

    if (endOfHostLine != std::string::npos) {
      // Extract the original Host line
      std::string originalHostLine = buffer.substr(hostPosition, endOfHostLine - hostPosition);

      // Create the new Host line
      std::string newHostLine = "Host: " + newHost;

      // Replace the original Host line with the new one
      buffer.replace(hostPosition, originalHostLine.length(), newHostLine);
    }
  }

  return buffer;
}


Proxy::~Proxy(){
  std::cout << "[-] Shutdown Proxy" << std::endl << std::endl;;
  shutdown(server_fd, SHUT_RDWR);
}

std::string Proxy::loadFile(std::string path){
  std::string content = "";
  std::ifstream file;
  file.open (path, std::ifstream::in);
  if(file.good()){
    content = std::string((std::istreambuf_iterator<char>(file) ), (std::istreambuf_iterator<char>()));
  }else{
    throw std::exception();
  }
  file.close();
  return content;
}

int Proxy::checkHost(std::string* host) {
  std::string blacklist = loadFile(dir + "/blacklist");
  std::string redirect = loadFile(dir + "/redirect");

  std::vector<std::string> blacklistLines;
  std::vector<std::string> redirectLines;

  // Split the blacklist and redirect strings into lines
  size_t pos = 0;
  while ((pos = blacklist.find('\n')) != std::string::npos) {
    blacklistLines.push_back(blacklist.substr(0, pos));
    blacklist.erase(0, pos + 1);
  }
  blacklistLines.push_back(blacklist);

  pos = 0;
  while ((pos = redirect.find('\n')) != std::string::npos) {
    redirectLines.push_back(redirect.substr(0, pos));
    redirect.erase(0, pos + 1);
  }
  redirectLines.push_back(redirect);

  for (const std::string& line : blacklistLines) {
    if (!line.empty() && line[0] != '#') {
      if (line == *host) {
        *host = "blacklist";
        return 1;
      }
    }
  }

  for (const std::string& line : redirectLines) {
    if (!line.empty() && line[0] != '#') {
      size_t pos = line.find('>');
      if (pos != std::string::npos) {
        std::string fromHost = line.substr(0, pos);
        std::string toHost = line.substr(pos + 1);
        if (fromHost == *host) {
          *host = toHost;
          return 2;
        }
      }
    }
  }

  return 0;
}


std::string Proxy::getHost(std::string buffer){
  size_t hostPosition = buffer.find("Host:");

  if (hostPosition != std::string::npos) {
    size_t endOfHostLine = buffer.find("\r\n", hostPosition);

    if (endOfHostLine != std::string::npos) {
      // Extract the original Host line
      std::string originalHostLine = buffer.substr(hostPosition + 6, endOfHostLine - hostPosition-6);
      return originalHostLine;
    }
    return "";
  }
  return "";
}


std::string Proxy::getRequestPath(std::string buffer){ // "GET / HTTP/1.1 \n...\n.."
  int cnt = 0;
  for(size_t i = 0; i < buffer.length(); i++)
    if(buffer[i] == '\n')
      break;
    else
      cnt++;

  std::string firstline = buffer.substr(0, cnt); // "GET / HTTP/1.1"
  cnt = 0;
  bool found = false;
  int start = 0;
  for(size_t i = 0; i < firstline.length(); i++)
     if(buffer[i] == ' ')
        if(!found){
          found = true; // found first space after GET
          start = cnt;
        } else {
          break;        // found second space after path
        }
     else
        cnt++;

  std::string path = firstline.substr(start, cnt);
  return path;
}

std::vector<std::string> Proxy::customSplit(std::string str, char separator){
std::vector<std::string> result;
std::string::size_type startIndex = 0, endIndex = 0;

for (std::string::size_type i = 0; i <= str.size(); i++) {
if (i == str.size() || str[i] == separator) {
endIndex = i;
std::string temp;
temp.append(str, startIndex, endIndex - startIndex);
result.push_back(temp);
startIndex = endIndex + 1;
}
}

return result;
}