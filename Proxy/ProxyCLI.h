#pragma once
#include <string>
#include <filesystem>
#include <iostream>

class ProxyCLI{
  std::string dir = "rules";
  int port = 8000;

  void print_help(std::string binary); 
public:
  ProxyCLI(int argc, char* argv[]);
  std::string getDirectory();
  int getPort();
};
