#include <string>

#include "parser.h"

int main(int argc, char** argv) {
  if (argc > 1) {
    Parser parser((std::string)argv[1]);
  }
}
