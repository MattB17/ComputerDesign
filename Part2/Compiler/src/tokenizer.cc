#include "tokenizer.h"

Tokenizer::Tokenizer(std::string jack_file) : token_type_(TokenType::UNKNOWN) {
  jack_stream_.open(jack_file);
}

bool Tokenizer::hasMoreTokens() {
  while (!jack_stream_.eof()) {
    char next_char = jack_stream_.peek();
    if (isTokenBeginningChar(next_char) || IsSymbol(next_char)) {
      return true;
    }
    if (std::isspace(static_cast<unsigned char>(next_char))) {
      char curr_char;
      jack_stream_.get(curr_char);
    } else {
      removeComment(next_char);
    }
  }
  return false;
}

void Tokenizer::advance() {
  char next_char = jack_stream_.peek();
  // We are dealing with a symbol.
  if (IsSymbol(next_char)) {
    token_type_ = TokenType::SYMBOL;
    // It is just a single character, so put it in the stream and return.
    token_stream_ << jack_stream_.get();
    return;
  }
  // we are dealing with an integer.
  if (isalnum(next_char)) {
    token_type_ = TokenType::INT_CONST;
    token_stream_ << jack_stream_.get();
    // keep parsing until we encounter a space or symbol.
    while(!jack_stream_.eof()) {
      next_char = jack_stream_.peek();
      if (isalnum(next_char)) {
        token_stream_ << jack_stream_.get();
      } else if (IsSymbol(next_char) ||
                 std::isspace(static_cast<unsigned char>(next_char))) {
        break;
      }
    }
    return;
  }
  // we are dealing with a string.
  if (next_char == '"') {
    token_type_ = TokenType::STRING_CONST;
    char temp_char;
    // throw away the opening quote.
    jack_stream_.get(temp_char);
    if (!jack_stream_.eof()) {
      jack_stream_.get(temp_char);
      // if its not the ending quote then it is part of the string.
      while ((temp_char != '"') && (!jack_stream_.eof())) {
        token_stream_ << temp_char;
        jack_stream_.get(temp_char);
      }
    }
    return;
  }
  // otherwise, we have a keyword or identifier but we need to parse the
  // token to figure out which.
  token_stream_ << jack_stream_.get();
  while (!jack_stream_.eof()) {
    next_char << jack_stream_.peek();
    if (isalnum(next_char)) {
      token_stream_ << jack_stream_.get();
    } else if (IsSymbol(next_char) ||
               std::isspace(static_cast<unsigned char>(next_char))) {
      break;
    }
  }
  if (IsKeyword(token_stream_.str())) {
    token_type_ = TokenType::KEYWORD;
  } else {
    token_type_ = TokenType::IDENTIFIER;
  }
  return;
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

void Tokenizer::removeComment(char next_char) {
  char curr_char;
  if (next_char == '/') {
    // throw it away, we know we're dealing with a comment.
    jack_stream_.get(curr_char);
    next_char = jack_stream_.peek();
    if (next_char == '/') {
      // this means we have an inline comment. So just move to next line.
      std::string line;
      std::getline(jack_stream_, line);
    } else if (next_char == '*') {
      // we are dealing with a block comment. So we keep parsing until we
      // find '*/'.
      jack_stream_.get(curr_char);
      bool found_comment_end = false;
      while ((!found_comment_end) && (!jack_stream_.eof())) {
        jack_stream_.get(curr_char);
        if (curr_char == '*') {
          jack_stream_.get(curr_char);
          found_comment_end = (curr_char == '/');
        }
      }
    }
  }
}
