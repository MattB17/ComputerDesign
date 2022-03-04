#include "parser.h"

Parser::Parser(std::string vm_file) : curr_command_type_(Operation::UNKNOWN),
                                      arg1_(""),
                                      arg2_(-1)
{
  vm_stream_.open(vm_file);
}

Parser::~Parser() { vm_stream_.close(); }
