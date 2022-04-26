#include "tokenizer.h"

Tokenizer::Tokenizer(std::string jack_file) : token_type_(TokenType::UNKNOWN) {
  jack_stream_.open(jack_file);
}

const Keyword Tokenizer::getKeyword() {
  return GetKeywordFromString(token_stream_.str());
}

const char Tokenizer::getSymbol() {
  return token_stream_.getc();
}

const std::string Tokenizer::getIdentifier() {
  return token_stream_.str();
}

const std::string Tokenizer::getIntVal() {
  int int_val;
  token_stream_ >> int_val;
  return int_val;
}

// Note, when we encounter a stream we just don't add the '"' to the stream
// so we can just directly cast it.
const std::string Tokenizer::getStringVal() {
  return token_stream_.str();
}
