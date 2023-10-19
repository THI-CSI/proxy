#pragma once
#include <string>
#include <filesystem>
#include <iostream>

class WebserverCLI{
  std::string dir = "public";
  int port = 8000;

  void print_help(std::string binary); 
public:
  WebserverCLI(int argc, char* argv[]);
  std::string getDirectory();
  int getPort();
};
