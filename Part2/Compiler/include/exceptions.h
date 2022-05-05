// Implements a collection of exceptions used throughout the project.
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>

#include "keyword.h"
#include "util.h"

class InvalidTerm : public std::runtime_error {
public:
  explicit InvalidTerm(std::string received_token) : std::runtime_error(
    "A valid term is one of an integer constant, a string constant, a valid "
    "identifier, or a keyword constant: `true`, `false`, `null`, or `this`. "
    "Instead received " + received_token + ".")
  {}
};

class InvalidClassVarKeyword : public std::runtime_error {
public:
  explicit InvalidClassVarKeyword(std::string received_token)
    : std::runtime_error("Expected keyword `static` or `field` for class "
                         "variable declaration. Instead received " +
                         received_token + ".") {}
};

class ExpectedClosingParenthesis : public std::runtime_error {
public:
  ExpectedClosingParenthesis(
    std::string expected_parenthesis, std::string received_token)
    : std::runtime_error("Expected " + expected_parenthesis + ". Instead "
                         "received " + received_token + ".") {}
};

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
  KeywordNotFound(Keyword::Type expected_keyword, std::string received_token)
    : std::runtime_error("Expected to receive keyword " +
                         Keyword::KeywordToString(expected_keyword) +
                         ", but instead got " + received_token + ".") {}
};

class InvalidToken : public std::runtime_error {
public:
  explicit InvalidToken(std::string start_char) : std::runtime_error(
    "Tokens must be a symbol, a string, an integer, or a valid identifier "
    " that does not start with a number. Instead received a token starting "
    "with " + start_char + ".") {}
};

class InvalidIdentifier : public std::runtime_error {
public:
  explicit InvalidIdentifier(std::string start_char) : std::runtime_error(
    "Identifiers must contain only '_' or alphanumerics, and must not start "
    "with a number. Instead, received an identifier starting with " +
    start_char + ".") {}
};

class NonTerminatedString : public std::exception {
  virtual const char* what() const throw() {
    return "Non Terminated String: No closing quotation for string.";
  }
};

#endif  // EXCEPTIONS_H
