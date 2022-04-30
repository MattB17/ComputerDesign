// Implements a collection of exceptions used throughout the project.
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>

#include "keyword.h"
#include "util.h"

class ExpectedStatementEnd : public std::exception {
private:
  std::string received_token_;
public:
  explicit ExpectedStatementEnd(std::string received_token)
    : received_token_(received_token) {}

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << "Expected ';'. Instead, received " << received_token_ << ".";
    return streamToCharArray(ss);
  }
};

class MissingIdentifier : public std::exception {
private:
  std::string received_token_;
public:
  explicit MissingIdentifier(std::string received_token)
    : received_token_(received_token) {}

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << "Expected a valid identifier. Instead received ";
    ss << received_token_ << ".";
    return streamToCharArray(ss);
  }
};

class InvalidType : public std::exception {
private:
  std::string received_token_;
public:
  explicit InvalidType(std::string received_token)
    : received_token_(received_token) {}

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << "Expected a valid type. Instead received ";
    ss << received_token_ << ".";
    return streamToCharArray(ss);
  }
};

class KeywordNotFound : public std::exception {
private:
  Keyword expected_keyword_;
  std::string received_token_;
public:
  KeywordNotFound(Keyword expected_keyword, std::string received_token)
    : expected_keyword_(expected_keyword), received_token_(received_token) {}

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << "Keyword Not Found: Expected to receive keyword ";
    ss << keywordToString(expected_keyword_) << ", but instead got ";
    ss << received_token_ << ".";
    return streamToCharArray(ss);
  }
};

class InvalidToken : public std::exception {
private:
  char start_char_;
public:
  explicit InvalidToken(const char start_char) : start_char_(start_char) {}

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << "Invalid Token: tokens must be a symbol, a string, an integer or ";
    ss << "a valid identifier that does not start with a number.";
    ss << "Instead received a token starting with " << start_char_ << ".";
    return streamToCharArray(ss);
  }
};

class InvalidIdentifier : public std::exception {
private:
  char start_char_;
public:
  explicit InvalidIdentifier(const char start_char) : start_char_(start_char) {}
  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << "Invalid Identifier: identifiers must contain only '_' or ";
    ss << "alphanumerics, and must not start with a number. Instead, received ";
    ss << "an identifier starting with " << start_char_ << ".";
    return streamToCharArray(ss);
  }
};

class NonTerminatedString : public std::exception {
  virtual const char* what() const throw() {
    return "Non Terminated String: No closing quotation for string.";
  }
};

#endif  // EXCEPTIONS_H
