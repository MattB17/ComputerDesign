// Used to parse a `.vm` file and interpret the instructions, one at a time.
// It reads a VM command, parses the command into its lexical components, and
// provides convenient access to these components.
// Ignores all whitespaces and comments.
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <fstream>

#include "operation.h"

class Parser {
public:
  Parser(std::string vm_file);
  Parser(const Parser&) = delete;
  Parser &operator=(const Parser&) = delete;
  Parser(Parser&&) = delete;
  Parser &operator=(Parser&&) = delete;
  ~Parser();

  // determines if the parser has more commands
  bool hasMoreCommands();

  // advances to the next command and makes it the current command
  void advance();

  // identifies the type of the current command
  Operation commandType() { return curr_command_type_; }

  // retrieves the first argument of the current command
  // this should not be called if the command type is RETURN.
  std::string get_arg1() { return arg1_; }

  // retrieves the second argument of the current command
  // this should only be called in the command type is PUSH, POP, FUNCTION,
  // or CALL.
  int get_arg2() { return arg2_; }
private:
  std::ifstream vm_stream_;
  Operation curr_command_type_;
  std::string arg1_;
  int arg2_;
};

#endif  // PARSER_H
