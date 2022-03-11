#include <string>
#include <sstream>
#include <iostream>

#include "operation.h"
#include "parser.h"
#include "code_writer.h"

std::string getFilePathFromFullFileName(std::string full_file_name) {
  size_t extension_pos = full_file_name.find_last_of(".");
  return full_file_name.substr(0, extension_pos);
}

std::string getFileNameFromPathWithoutExtension(std::string file_path) {
  size_t name_pos = file_path.find_last_of("/\\");
  return file_path.substr(name_pos + 1);
}

std::string constructAssemblyFile(std::string file_path) {
  std::stringstream ss;
  ss << file_path << ".asm";
  return ss.str();
}

int main(int argc, char** argv) {
  if (argc > 1) {
    std::string vm_file = ((std::string)argv[1]);
    Parser parser(vm_file);

    std::string file_path = getFilePathFromFullFileName(vm_file);
    std::string file_name = getFileNameFromPathWithoutExtension(file_path);
    std::string assembly_path = constructAssemblyFile(file_path);

    CodeWriter code_writer(assembly_path, file_name);
    while (parser.hasMoreCommands()) {
      parser.advance();
      code_writer.writeCommandComment(parser.getCurrentCommand());
      if (parser.commandType() == Operation::PUSH ||
          parser.commandType() == Operation::POP) {
        code_writer.writePushPop(
          parser.commandType(), parser.getArg1(), parser.getArg2());
      } else if (parser.commandType() == Operation::ARITHMETIC) {
        code_writer.writeArithmetic(parser.getArg1());
      }
    }
    code_writer.close();
  }
  return 0;
}
