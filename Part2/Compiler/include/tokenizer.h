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

  // The type of the current token.
  const TokenType getTokenType() { return token_type_; }

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
  void removeComment();

  // The stream for the input file.
  std::ofstream jack_stream_;

  // The token type and a stream of characters representing the current token.
  TokenType token_type_;
  std::string_stream_ token_stream_;
};

#endif  // TOKENIZER_H
