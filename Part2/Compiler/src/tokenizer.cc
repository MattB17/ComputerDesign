#include "tokenizer.h"

#include "exceptions.h"
#include "symbol.h"
#include "util.h"

Tokenizer::Tokenizer(std::string jack_file) : token_type_(TokenType::UNKNOWN) {
  jack_stream_.open(jack_file);
}

bool Tokenizer::hasMoreTokens() {
  while (!jack_stream_.eof()) {
    char next_char = jack_stream_.peek();
    if (startsComment(next_char)) {
      removeComment(next_char);
    } else if (isTokenBeginningChar(next_char)) {
      return true;
    } else if (isSpaceChar(next_char)) {
      char curr_char;
      jack_stream_.get(curr_char);
    }
  }
  return false;
}

void Tokenizer::advance() {
  token_stream_.clear();
  token_stream_.str(std::string());
  char next_char = jack_stream_.peek();
  // We are dealing with a symbol.
  if (IsSymbol(next_char)) {
    token_type_ = TokenType::SYMBOL;
    // It is just a single character, so put it in the stream and return.
    token_stream_ << getNextCharFromJackStream();
    return;
  }
  // we are dealing with an integer.
  if (isdigit(next_char)) {
    token_type_ = TokenType::INT_CONST;
    token_stream_ << getNextCharFromJackStream();
    // keep parsing until we encounter a space or symbol.
    while(!jack_stream_.eof()) {
      next_char = jack_stream_.peek();
      if (isdigit(next_char)) {
        token_stream_ << getNextCharFromJackStream();
      } else if (IsSymbol(next_char) || isSpaceChar(next_char)) {
        break;
      } else {
        throw InvalidIdentifier(std::string(1, next_char));
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
      if (temp_char != '"') {
        throw NonTerminatedString();
      }
    }
    return;
  }
  // otherwise, we have a keyword or identifier but we need to parse the
  // token to figure out which.
  token_stream_ << getNextCharFromJackStream();
  while (!jack_stream_.eof()) {
    next_char = jack_stream_.peek();
    if (isIdentifierChar(next_char)) {
      token_stream_ << getNextCharFromJackStream();
    } else if (IsSymbol(next_char) || isSpaceChar(next_char)) {
      break;
    } else {
      throw InvalidIdentifier(std::string(1, next_char));
    }
  }
  if (Keyword::IsKeyword(token_stream_.str())) {
    token_type_ = TokenType::KEYWORD;
  } else {
    token_type_ = TokenType::IDENTIFIER;
  }
  return;
}

bool Tokenizer::nextToken() {
  if (!hasMoreTokens()) {
    token_type_ = TokenType::UNKNOWN;
    token_stream_.clear();
    token_stream_.str(std::string());
    return false;
  }
  advance();
  return true;
}

std::string Tokenizer::tokenToString() {
  if (token_type_ == TokenType::SYMBOL) {
    const char symbol_char = token_stream_.str().c_str()[0];
    return SymbolToString(symbol_char);
  }
  return token_stream_.str();
}

const Keyword::Type Tokenizer::getKeyword() {
  return Keyword::GetKeywordFromString(token_stream_.str());
}

const char Tokenizer::getSymbol() {
  return token_stream_.str().c_str()[0];
}

const std::string Tokenizer::getIdentifier() {
  return token_stream_.str();
}

const int Tokenizer::getIntVal() {
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

char Tokenizer::getNextCharFromJackStream() {
  return static_cast<char>(jack_stream_.get());
}

bool Tokenizer::startsComment(char next_char) {
  if (next_char != '/') {
    return false;
  }
  jack_stream_.seekg(1, jack_stream_.cur);
  char second_char = jack_stream_.peek();
  jack_stream_.seekg(-1, jack_stream_.cur);
  if ((second_char == '/') || (second_char == '*')) {
    return true;
  }
  return false;
}
