#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <string>
#include "webserver.h"
#include "cli.h"

int main (int argc, char *argv[]) {
  WebserverCLI cli(argc, argv);
  Webserver webserver(cli.getPort(), cli.getDirectory());
  printf("[+] Starting webserver on port '%d' in the directory '%s'.\n", cli.getPort(), cli.getDirectory().c_str());
  webserver.init();
  return 0;
}

