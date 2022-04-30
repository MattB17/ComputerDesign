// Used to serialize an input stream into a sequence of Jack-language tokens.
// These tokens are then used by the syntax analyzer to translate to VM code.
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <fstream>
#include <sstream>
#include <string>

#include "keyword.h"
#include "token_type.h"

class Tokenizer {
public:
  Tokenizer(std::string jack_file);
  Tokenizer(const Tokenizer&) = delete;
  Tokenizer &operator=(const Tokenizer&) = delete;
  Tokenizer(Tokenizer&&) = delete;
  Tokenizer &operator=(Tokenizer&&) = delete;
  ~Tokenizer() {}

  // Determines if the stream has any more tokens.
  bool hasMoreTokens();

  // Advances the tokenizer and makes the next token in the file stream the
  // current token. This should only be called if `hasMoreTokens` returns true.
  void advance();

  // Advances the tokenizer to the next token if one exists and returns true.
  // Otherwise, returns false.
  bool nextToken();

  // The type of the current token.
  const TokenType getTokenType() { return token_type_; }

  // Gets the current token as a string.
  std::string tokenToString() { return token_stream_.str(); }

  // Returns the keyword corresponding to the current token.
  // Should only be called if the token type is KEYWORD.
  const Keyword getKeyword();

  // Returns the symbol corresponding to the current token.
  // Should only be called if the token type is SYMBOL.
  const char getSymbol();

  // Returns the identifier corresponding to the current token.
  // Should only be called if the token type is IDENTIFIER.
  const std::string getIdentifier();

  // Returns the integer corresponding to the current token.
  // Should only be called if the token type is INT_CONST.
  const int getIntVal();

  // Returns the string corresponding to the current token.
  // Should only be called if the token type is STRING_CONST.
  const std::string getStringVal();
private:
  // Removes the current comment pointed to by the jack_stream_.
  void removeComment(char next_char);

  // retrieves the next character from the jack stream.
  char getNextCharFromJackStream();

  // Identifies whether `next_char` is the first character of a comment.
  bool startsComment(char next_char);

  // The stream for the input file.
  std::ifstream jack_stream_;

  // The token type and a stream of characters representing the current token.
  TokenType token_type_;
  std::stringstream token_stream_;
};

#endif  // TOKENIZER_H
