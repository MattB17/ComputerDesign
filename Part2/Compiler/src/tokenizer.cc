#include "tokenizer.h"

Tokenizer::Tokenizer(std::string jack_file)
  : token_type_(TokenType::UNKNOWN), token_("") {
  jack_stream_.open(jack_file);
}

const Keyword Tokenizer::getKeyword() {
  return Keyword::GetKeywordFromString(token_);
}
