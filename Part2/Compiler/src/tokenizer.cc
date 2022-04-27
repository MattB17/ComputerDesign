#include "tokenizer.h"

Tokenizer::Tokenizer(std::string jack_file) : token_type_(TokenType::UNKNOWN) {
  jack_stream_.open(jack_file);
}

bool Tokenizer::hasMoreTokens() {
  while (!jack_stream_.eof()) {
    char next_char = jack_stream_.peek();
    if (isTokenBeginningChar(next_char)) {
      return true;
    }
    char curr_char;
    if (std::isspace(static_cast<unsigned char>(next_char))) {
      curr_char << jack_stream_;
    } else {
      removeComment();
    }
  }
  return false;
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

// Note, when we encounter a string we just don't add the '"' to the stream
// so we can just directly cast it.
const std::string Tokenizer::getStringVal() {
  return token_stream_.str();
}

void Tokenizer::removeComment() {
  if (next_char == '/') {
    // throw it away, we know we're dealing with a comment.
    curr_char << jack_stream_;
    next_char = jack_stream_.peek();
    if (next_char == '/') {
      // this means we have an inline comment. So just move to next line.
      std::string line;
      std::getline(jack_stream_, line);
    } else if (next_char == '*') {
      // we are dealing with a block comment. So we keep parsing until we
      // find '*/'.
      curr_char << jack_stream_;
      bool found_comment_end = false;
      while ((!found_comment_end) && (!jack_stream_.eof())) {
        curr_char << jack_stream_;
        if (curr_char == '*') {
          curr_char << jack_stream_;
          found_comment_end = (curr_char == '/');
        }
      }
    }
  }
}
