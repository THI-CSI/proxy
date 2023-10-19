#include "server.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <fstream>

Webserver::Webserver(int port, std::string directory){
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  if(server_fd < 0){
    std::cerr << "[-] Socket failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
    std::cerr << "[-] Socket configuration failed" << std::endl;
    exit(EXIT_FAILURE);
  }

  if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
    std::cerr << "[-] Bind connection failed" << std::endl;
    exit(EXIT_FAILURE);
  }
  
}

void Webserver::init(){
  int new_socket;
  while(true){
    if(listen(server_fd, 3) < 0){
      std::cerr << "[-] Listen to port failed" << std::endl;
      exit(EXIT_FAILURE);
    }
    
    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&address_len);
    if(new_socket <0){
      std::cerr << "[-] Could not accept the connection" << std::endl;
      exit(EXIT_FAILURE);
    }

    read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    std::string body = "<h1>Hello World</h1>";
    std::string content_type = getContentType("index.html");
    std::string payload = getHeader(content_type, body.length(), 200) + body;
    send(new_socket, payload.c_str(), payload.length(), 0);
    close(new_socket);
  }
}

Webserver::~Webserver(){
  shutdown(server_fd, SHUT_RDWR);
}


std::string Webserver::getHeader(std::string content_type, int body_len, int status_code){
  int length = body_len + 55 + content_type.length();
  std::string header = 
    "HTTP/1.1 " + std::to_string(status_code) + "\r\n"
    "Content-Type: "+ content_type + "\r\n"
    "Content-Length: " + std::to_string(length) + "\r\n\r\n";
  return header;
}

std::string Webserver::getContentType(std::string path){
  std::string extension = "";
  size_t pos = path.find_last_of('.');
  if (pos != std::string::npos) {
    extension = path.substr(pos + 1);
  }  

  if(extension == "html")
    return "text/html";
  else if(extension == "css")
    return "text/css";
  else if(extension == "js")
    return "text/javascript";
  else if(extension == "ico")
    return "image/x-icon";
  return "text/plain";
}

std::string loadFile(std::string path){
  return "";
}

