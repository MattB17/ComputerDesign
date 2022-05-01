// Implements a collection of exceptions used throughout the project.
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>

#include "keyword.h"
#include "util.h"

class ExpectedStatementEnd : public std::runtime_error {
public:
  explicit ExpectedStatementEnd(std::string received_token)
    : std::runtime_error("Expected ';'. Instead received " +
                         received_token + ".") {}
};

class MissingIdentifier : public std::runtime_error {
public:
  explicit MissingIdentifier(std::string received_token)
    : std::runtime_error("Expected a valid identifier. Instead received " +
                         received_token + ".") {}
};

class InvalidType : public std::runtime_error {
public:
  explicit InvalidType(std::string received_token)
    : std::runtime_error("Expected a valid type. Instead received " +
                         received_token + ".") {}
};

class KeywordNotFound : public std::runtime_error {
public:
  KeywordNotFound(Keyword expected_keyword, std::string received_token)
    : std::runtime_error("Expected to receive keyword " +
                         keywordToString(expected_keyword) +
                         ", but instead got " + received_token + ".") {}
};

class InvalidToken : public std::runtime_error {
public:
  explicit InvalidToken(const char start_char) : std::runtime_error(
    "Tokens must be a symbol, a string, an integer, or a valid identifier " +
    " that does not start with a number. Instead received a token starting " +
    "with " + start_char + ".") {}
};

class InvalidIdentifier : public std::runtime_error {
public:
  explicit InvalidIdentifier(const char start_char) : std::runtime_error(
    "Identifiers must contain only '_' or alphanumerics, and must not start " +
    "with a number. Instead, received an identifier starting with " +
    start_char + ".") {}
};

class NonTerminatedString : public std::exception {
  virtual const char* what() const throw() {
    return "Non Terminated String: No closing quotation for string.";
  }
};

#endif  // EXCEPTIONS_H
