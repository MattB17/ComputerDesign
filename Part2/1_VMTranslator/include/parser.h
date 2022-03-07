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

  // the raw VM operation representing the current command
  std::string getCurrentCommand() { return curr_command_; }

  // identifies the type of the current command
  Operation commandType() { return command_type_; }

  // retrieves the first argument of the current command
  // this should not be called if the command type is RETURN.
  std::string getArg1() { return arg1_; }

  // retrieves the second argument of the current command
  // this should only be called in the command type is PUSH, POP, FUNCTION,
  // or CALL.
  int getArg2() { return arg2_; }

private:
  void getCurrCommandComponents();
  // the input file stream
  std::ifstream vm_stream_;

  // identifies the raw text of the current command
  std::string curr_command_;

  // the components of the current command
  Operation command_type_;
  std::string arg1_;
  int arg2_;
};

#endif  // PARSER_H
