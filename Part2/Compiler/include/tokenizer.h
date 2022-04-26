// Used to serialize an input stream into a sequence of Jack-language tokens.
// These tokens are then used by the syntax analyzer to translate to VM code.
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <fstream>
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

  const TokenType getTokenType() { return token_type_; }

  const Keyword getKeyword();
private:
  // The stream for the input file.
  std::ofstream jack_stream_;

  // The token type and a string representing the current token.
  TokenType token_type_;
  std::string token_;
};

#endif  // TOKENIZER_H
