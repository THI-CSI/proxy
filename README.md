# Simple Webserver
A simple C++ Webserver.

## Usage
```shell
usage: webserver [--port port | --directory directory]

  -h, --help         Show this help page
  -p, --port         Select the port
  -d, --directory    Select the directory to serve
```

## Build Instructions
Prerequisites:
```shell
sudo apt install build-essential 
```

Build the webserver binary using Make:
```shell
make 
```
## Build with Nix
Get the derivation from [here](https://github.com/bee1850/my-nix-packages/blob/main/simple-custom-webserver/default.nix) and build with `nix-build`.

## References
- [Socket Programming (geeksforgeeks.org)](https://www.geeksforgeeks.org/socket-programming-cc/)
- [Example Hello World HTML File (learn-html.org)](https://www.learn-html.org/en/Hello%2C_World%21)
