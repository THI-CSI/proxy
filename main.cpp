#include <iostream>
#include "server.h"

int main (int argc, char *argv[]) {
  Webserver webserver(8080, "./public");
  webserver.init();
  return 0;
}

