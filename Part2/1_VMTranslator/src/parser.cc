#include "parser.h"

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
    std::string vm_op;
    vm_stream_ >> vm_op;
    curr_command_type_ = GetOperationFromString(vm_op);
    if (curr_command_type_ == Operation::ARITHMETIC) {
      arg1_ = vm_op;
      return;
    }
    if (!IsOperationWithNoArguments(curr_command_type_)) {
      vm_stream_ >> arg1_;
    }
    if (IsOperationWithTwoArguments(curr_command_type_)) {
      vm_stream_ >> arg2_;
    }
  }
}
