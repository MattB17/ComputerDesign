// An Enum representing the collection of token types for the jack language.
#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

enum TokenType {
  // The set of possible token types
  KEYWORD = 0,
  SYMBOL = 1,
  IDENTIFIER = 2,
  INT_CONST = 3,
  STRING_CONST = 4,
  UNKNOWN = 5
};

bool isTokenBeginningChar(char curr_char) {
  return (isalnum(curr_char) || (curr_char == '"') || (curr_char == "_"));
}

#endif  // TOKEN_TYPE_H
