#include "WebserverCLI.h"

void WebserverCLI::print_help(std::string binary){
  std::cout << "usage: " << binary << " [--port port | --directory directory]" << std::endl << std::endl;
  std::cout << "  -h, --help         Show this help page" << std::endl;
  std::cout << "  -p, --port         Select the port" << std::endl;
  std::cout << "  -d, --directory    Select the directory to serve" << std::endl;
  exit(-1);
}

WebserverCLI::WebserverCLI(int argc, char* argv[]){
  if(argc <= 1){
    return;
  }

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
          this->port = input;
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
        this->dir = std::string(argv[i+1]);
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


std::string WebserverCLI::getDirectory(){
  return this->dir;
}

int WebserverCLI::getPort(){
  return this->port;
}
