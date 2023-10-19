#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include "Webserver/Webserver.h"
#include "Webserver/WebserverCLI.h"

int main (int argc, char *argv[]) {
  WebserverCLI cli(argc, argv);
  Webserver webserver(cli.getPort(), cli.getDirectory());
  printf("[+] Starting webserver on port '%d' in the directory '%s'.\n", cli.getPort(), cli.getDirectory().c_str());
  printf("[!] Link: http://127.0.0.1:%d/\n", cli.getPort());
  webserver.init();
  return 0;
}

