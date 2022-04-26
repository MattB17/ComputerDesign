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

  bool hasMoreTokens();

  const TokenType getTokenType() { return token_type_; }

  const Keyword getKeyword();

  const char getSymbol();

  const std::string getIdentifier();

  const int getIntVal();

  const std::string getStringVal();
private:
  // The stream for the input file.
  std::ofstream jack_stream_;

  // The token type and a stream of characters representing the current token.
  TokenType token_type_;
  std::string_stream_ token_stream_;
};

#endif  // TOKENIZER_H
