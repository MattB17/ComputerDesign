// Contains a collection of static methods for dealing with symbols.
#ifndef SYMBOL_H
#define SYMBOL_H

#include <unordered_set>

static std::unordered_set<char> const symbols = {
  '{', '}', '(', ')', '[', ']', '.', ',', ';',
  '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'
};

static bool IsSymbol(const char curr_char) {
  return (symbols.find(curr_char) != symbols.end());
}

#endif  // SYMBOL_H
