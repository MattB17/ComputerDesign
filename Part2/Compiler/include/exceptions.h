// Implements a collection of exceptions used throughout the project.
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

class InvalidToken : public std::exception {
  virtual const char* what() const throw() {
    return "Invalid Token: tokens must be a symbol, a string, an integer or "
           "a valid identifier that does not start with a number.";
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
