#include "parser.h"

#include <sstream>

Parser::Parser(std::string vm_file) : curr_command_type_(Operation::UNKNOWN),
                                      arg1_(""),
                                      arg2_(-1)
{
  vm_stream_.open(vm_file);
}

Parser::~Parser() { vm_stream_.close(); }

bool Parser::hasMoreCommands() { return !vm_stream_.eof(); }

void Parser::advance() {
  if (!vm_stream_.eof()) {
    std::getline(vm_stream_, curr_command_);
    getCurrCommandComponents();
  }
}

void Parser::getCurrCommandComponents() {
  std::istringstream command_stream(curr_command_);
  std::string vm_op;
  command_stream >> vm_op;
  curr_command_type_ = GetOperationFromString(vm_op);
  if (curr_command_type_ == Operation::ARITHMETIC) {
    arg1_ = vm_op;
    return;
  }
  if (!IsOperationWithNoArguments(curr_command_type_)) {
    curr_command_ >> arg1_;
  }
  if (IsOperationWithTwoArguments(curr_command_type_)) {
    curr_command_ >> arg2_;
  }
}
