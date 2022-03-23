#include <string>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <utility>
#include <vector>

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

    std::vector<std::pair<std::string, std::string>> vm_name_path_pairs;
    std::string file_path = vm_file;
    size_t extension_pos = vm_file.find_last_of(".vm");
    if (extension_pos == vm_file.size() - 1) {
      // is single_file
      file_path = vm_file.substr(0, extension_pos - 2);
      vm_name_path_pairs.push_back(std::make_pair(
        getFileNameFromPathWithoutExtension(file_path), vm_file));
    } else {
      // is directory
    }

    std::string assembly_path = constructAssemblyFile(file_path);
    CodeWriter code_writer(assembly_path);
    Parser parser;

    for (size_t i = 0; i < vm_name_path_pairs.size(); i++) {
      parser.openFile(vm_name_path_pairs[i].second);
      code_writer.setFileName(vm_name_path_pairs[i].first);

      while (parser.hasMoreCommands()) {
        parser.advance();
        code_writer.writeCommandComment(parser.getCurrentCommand());
        if (parser.commandType() == Operation::PUSH ||
            parser.commandType() == Operation::POP) {
          code_writer.writePushPop(
            parser.commandType(), parser.getArg1(), parser.getArg2());
        } else if (parser.commandType() == Operation::ARITHMETIC) {
          code_writer.writeArithmetic(parser.getArg1());
        } else if (parser.commandType() == Operation::LABEL) {
          code_writer.writeLabel(parser.getArg1());
        } else if (parser.commandType() == Operation::GOTO) {
          code_writer.writeGoTo(parser.getArg1());
        } else if (parser.commandType() == Operation::IF) {
          code_writer.writeIf(parser.getArg1());
        } else if (parser.commandType() == Operation::FUNCTION) {
          code_writer.writeFunction(parser.getArg1(), parser.getArg2());
        } else if (parser.commandType() == Operation::RETURN) {
          code_writer.writeReturn();
        } else if (parser.commandType() == Operation::CALL) {
          code_writer.writeCall(parser.getArg1(), parser.getArg2());
        }
      }
      parser.closeFile();
    }
    code_writer.close();
  }
  return 0;
}
