// Used to serialize an input stream into a sequence of Jack-language tokens.
// These tokens are then used by the syntax analyzer to translate to VM code.
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <fstream>
#include <sstream>
#include <string>

#include "token_type.h"

class Tokenizer {
public:
  Tokenizer(std::string jack_file);
  Tokenizer(const Tokenizer&) = delete;
  Tokenizer &operator=(const Tokenizer&) = delete;
  Tokenizer(Tokenizer&&) = delete;
  Tokenizer &operator=(Tokenizer&&) = delete;
  ~Tokenizer() {}
private:
  // The stream for the input file.
  std::ofstream jack_stream_;

  // The token type and a stream of characters representing the current token.
  TokenType token_type_;
  std::stringstream token_stream_;
};

#endif  // TOKENIZER_H
