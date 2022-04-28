// Contains a set of utility functions used throughout the compiler.
#ifndef UTIL_H
#define UTIL_H

#include <ctype.h>

#include "symbol.h"

bool isIdentifierChar(char curr_char) {
  return (isalnum(curr_char) || (curr_char == '_'));
}

bool isIdentifierStartChar(char curr_char) {
  return (isalpha(curr_char) || (curr_char == '_'));
}

bool isTokenBeginningChar(char curr_char) {
  return (isIdentifierChar(curr_char) ||
          (curr_char == '"') ||
          IsSymbol(curr_char));
}

bool isSpaceChar(char curr_char) {
  return std::isspace(static_cast<unsigned char>(curr_char));
}

#endif  // UTIL_H
