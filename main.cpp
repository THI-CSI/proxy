#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <string>
#include "server.h"

void print_help(std::string binary){
  std::cout << "usage: " << binary << " [--port port | --directory directory]" << std::endl << std::endl;
  std::cout << "  -h, --help         Show this help page" << std::endl;
  std::cout << "  -p, --port         Select the port" << std::endl;
  std::cout << "  -d, --directory    Select the directory to serve" << std::endl;
  exit(-1);
}

int main (int argc, char *argv[]) {
  std::string path = "./public";
  int port = 8080;
  if(argc > 1){
    std::string binary = std::string(argv[0]);
    std::string arg;
    for(int i = 1; i < argc; i++){
      arg = std::string(argv[i]);

      if(arg == "--port" || arg == "-p"){
        if(i >= argc)
          print_help(binary);
        
        int input;
        try{
          input = std::stoi(argv[i+1]);
          if(input >= 1024 && input <= 65536){
            port = input;
            i++;
            continue;
          }
        }catch(...){}
        std::cerr << "[-] Invalid port '" << argv[i+1] << "' (1024-65536)." << std::endl;
        exit(-1);
      
      }else if(arg == "--directory" || arg == "-d"){
        if(i >= argc)
          print_help(binary);

        std::filesystem::directory_entry dir(argv[i+1]);
        if(dir.is_directory()){
          path = std::string(argv[i+1]);
          i++;
          continue;
        }
        std::cerr << "[-] Invalid directory '" << argv[i+1] << "'." << std::endl;
        exit(-1);

      }else{
        print_help(binary);
      }
    }
  }
  Webserver webserver(port, path);
  printf("[+] Starting webserver on port '%d' in the directory '%s'.\n", port, path.c_str());
  webserver.init();
  return 0;
}

