#include "parser.h"

#include <sstream>

Parser::Parser()
  : curr_command_(""), command_type_(Operation::UNKNOWN), arg1_(""), arg2_(-1)
{}

void Parser::openFile(std::string vm_file) {
  vm_stream_.open(vm_file);
}

void Parser::closeFile() {
  vm_stream_.close();
  vm_stream_.clear();
}

bool Parser::hasMoreCommands() {
  while (!vm_stream_.eof()) {
    if (isalnum(vm_stream_.peek())) {
      return true;
    }
    std::string line;
    std::getline(vm_stream_, line);
  }
  return false;
}

void Parser::advance() {
  std::getline(vm_stream_, curr_command_);
  getCurrCommandComponents();
}

void Parser::getCurrCommandComponents() {
  std::istringstream command_stream(curr_command_);
  std::string vm_op;
  command_stream >> vm_op;
  command_type_ = GetOperationFromString(vm_op);
  if (command_type_ == Operation::ARITHMETIC) {
    arg1_ = vm_op;
    return;
  }
  if (!IsOperationWithNoArguments(command_type_)) {
    command_stream >> arg1_;
  }
  if (IsOperationWithTwoArguments(command_type_)) {
    command_stream >> arg2_;
  }
}
