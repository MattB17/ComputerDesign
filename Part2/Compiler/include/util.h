// Contains a set of utility functions used throughout the compiler.
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <ctype.h>
#include <sstream>
#include <string>

#include "symbol.h"

static bool isIdentifierChar(char curr_char) {
  return (isalnum(curr_char) || (curr_char == '_'));
}

static bool isIdentifierStartChar(char curr_char) {
  return (isalpha(curr_char) || (curr_char == '_'));
}

static bool isTokenBeginningChar(char curr_char) {
  return (isIdentifierChar(curr_char) ||
          (curr_char == '"') ||
          IsSymbol(curr_char));
}

static bool isSpaceChar(char curr_char) {
  return std::isspace(static_cast<unsigned char>(curr_char));
}

static std::string jackFileToXmlFile(std::string jack_file) {
  size_t extension_pos = jack_file.find_last_of(".jack");
  std::string file_name = jack_file.substr(0, extension_pos - 4);
  std::stringstream ss;
  ss << file_name << ".xml";
  return ss.str();
}

static const char* streamToCharArray(std::stringstream& ss) {
  return ss.str().c_str();
}

#endif  // UTIL_H