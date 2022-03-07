#include <string>
#include <iostream>

#include "parser.h"

int main(int argc, char** argv) {
  if (argc > 1) {
    Parser parser((std::string)argv[1]);
    while (parser.hasMoreCommands()) {
      parser.advance();
      std::cout << parser.getCurrentCommand() << std::endl;
    }
  }
  return 0;
}
