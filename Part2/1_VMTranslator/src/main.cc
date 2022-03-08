#include <string>
#include <iostream>

#include "operation.h"
#include "parser.h"
#include "code_writer.h"

int main(int argc, char** argv) {
  if (argc > 1) {
    Parser parser((std::string)argv[1]);
    CodeWriter code_writer((std::string)argv[2]);
    while (parser.hasMoreCommands()) {
      parser.advance();
      code_writer.writeCommandComment(parser.getCurrentCommand());
      if (parser.commandType() == Operation::PUSH) {
        code_writer.writePushPop(
          parser.commandType(), parser.getArg1(), parser.getArg2());
      }
    }
    code_writer.close();
  }
  return 0;
}
