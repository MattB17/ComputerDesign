// Implements a collection of exceptions used throughout the project.
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

#include "keyword.h"
#include "util.h"

class ExpectedSymbol : public std::runtime_error {
public:
  ExpectedSymbol(std::string received_token,
                 std::string expected_symbol,
                 std::string compile_tag);
};

class ExpectedOpeningParenthesis : public ExpectedSymbol {
public:
  ExpectedOpeningParenthesis(std::string received_token,
                             std::string expected_parenthesis,
                             std::string compile_tag);
};

class ExpectedClosingParenthesis : public ExpectedSymbol {
public:
  ExpectedClosingParenthesis(std::string received_token,
                             std::string expected_parenthesis,
                             std::string compile_tag);
};

class ExpectedStatementEnd : public ExpectedSymbol {
public:
  ExpectedStatementEnd(std::string received_token, std::string compile_tag);
};

class InvalidTerm : public std::runtime_error {
public:
  explicit InvalidTerm(std::string received_token);
};

class InvalidSubroutineDecKeyword : public std::runtime_error {
public:
  explicit InvalidSubroutineDecKeyword(std::string received_token);
};

class MissingIdentifier : public std::runtime_error {
public:
  explicit MissingIdentifier(std::string received_token);
};

class InvalidType : public std::runtime_error {
public:
  explicit InvalidType(std::string received_token);
};

class InvalidFunctionReturnType : public std::runtime_error {
public:
  explicit InvalidFunctionReturnType(std::string received_token);
};

class KeywordNotFound : public std::runtime_error {
public:
  KeywordNotFound(Keyword::Type expected_keyword, std::string received_token);
};

class InvalidToken : public std::runtime_error {
public:
  explicit InvalidToken(std::string start_char);
};

class InvalidIdentifier : public std::runtime_error {
public:
  explicit InvalidIdentifier(std::string start_char);
};

class NonTerminatedString : public std::exception {
  virtual const char* what() const throw() {
    return "Non Terminated String: No closing quotation for string.";
  }
};

#endif  // EXCEPTIONS_H
