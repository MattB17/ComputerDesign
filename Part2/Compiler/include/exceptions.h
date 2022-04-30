// Implements a collection of exceptions used throughout the project.
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <iostream>
#include <sstream>

#include "keyword.h"

class ExpectedStatementEnd : public std::exception {
  virtual const char* what() const throw() {
    return "Expected ';'.";
  }
}

class MissingIdentifier : public std::exception {
  virtual const char* what() const throw() {
    return "Expected a valid identifier.";
  }
};

class InvalidType : public std::exception {
  virtual const char* what() const throw() {
    return "Invalid Type";
  }
};

class KeywordNotFound : public std::exception {
private:
  Keyword expected_keyword_;
public:
  explicit KeywordNotFound(const Keyword expected_keyword)
    : expected_keyword_(expected_keyword) {}

  virtual const char* what() const throw() {
    std::stringstream ss;
    ss << "Keyword Not Found: Expected to receive keyword ";
    ss << keywordToString(expected_keyword_) << ".";
    const std::string tmp = ss.str();
    return tmp.c_str();
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
    const std::string tmp = ss.str();
    return tmp.c_str();
  }
};

class InvalidIdentifier : public std::exception {
  virtual const char* what() const throw() {
    return "Invalid Identifier: identifiers must contain only '_' or "
           "alphanumerics, and must not start with a number.";
  }
};

class NonTerminatedString : public std::exception {
  virtual const char* what() const throw() {
    return "Non Terminated String: No closing quotation for string.";
  }
};

#endif  // EXCEPTIONS_H
