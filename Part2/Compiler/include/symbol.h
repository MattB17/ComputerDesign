// Contains a collection of static methods for dealing with symbols.
#ifndef SYMBOL_H
#define SYMBOL_H

#include <unordered_set>

static std::unordered_set<char> const unaryOps = {'-', '~'};

static std::unordered_set<char> const binaryOps = {
  '+', '-', '*', '/', '&', '|', '<', '>', '='
};

static std::unordered_set<char> const otherSymbols = {
  '{', '}', '(', ')', '[', ']', '.', ',', ';'
};

static bool IsSymbol(const char curr_char) {
  return ((otherSymbols.find(curr_char) != otherSymbols.end()) ||
          (binaryOps.find(curr_char) != binaryOps.end()) ||
          (unaryOps.find(curr_char) != unaryOps.end()));
}

static bool IsUnaryOp(const char curr_char) {
  return (unaryOps.find(curr_char) != unaryOps.end());
}

static bool IsBinaryOp(const char curr_char) {
  return (binaryOps.find(curr_char) != binaryOps.end());
}

#endif  // SYMBOL_H
