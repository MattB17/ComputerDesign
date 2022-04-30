// An Enum representing the collection of keywords in the Jack language.
#ifndef KEYWORD_H
#define KEYWORD_H

#include <unordered_map>
#include <string>

enum class Keyword {
  // The set of possible keywords
  CLASS = 0,
  METHOD = 1,
  FUNCTION = 2,
  CONSTRUCTOR = 3,
  INT = 4,
  BOOLEAN = 5,
  CHAR = 6,
  VOID = 7,
  VAR = 8,
  STATIC = 9,
  FIELD = 10,
  LET = 11,
  DO = 12,
  IF = 13,
  ELSE = 14,
  WHILE = 15,
  RETURN = 16,
  TRUE = 17,
  FALSE = 18,
  NULL_VAL = 19,
  THIS = 20,
  UNKNOWN = 21
};

static std::unordered_map<std::string, Keyword> const keyword_map =
  {
    {"class", Keyword::CLASS},
    {"method", Keyword::METHOD},
    {"function", Keyword::FUNCTION},
    {"constructor", Keyword::CONSTRUCTOR},
    {"int", Keyword::INT},
    {"bool", Keyword::BOOLEAN},
    {"char", Keyword::CHAR},
    {"void", Keyword::VOID},
    {"var", Keyword::VAR},
    {"static", Keyword::STATIC},
    {"field", Keyword::FIELD},
    {"let", Keyword::LET},
    {"do", Keyword::DO},
    {"if", Keyword::IF},
    {"else", Keyword::ELSE},
    {"while", Keyword::WHILE},
    {"return", Keyword::RETURN},
    {"true", Keyword::TRUE},
    {"false", Keyword::FALSE},
    {"null", Keyword::NULL_VAL},
    {"this", Keyword::THIS}
  };

static std::string keywordToString(Keyword k) {
  switch (k) {
    case Keyword::CLASS:
      return "class";
    case Keyword::METHOD:
      return "method";
    case Keyword::FUNCTION:
      return "function";
    case Keyword::CONSTRUCTOR:
      return "constructor";
    case Keyword::INT:
      return "int";
    case Keyword::BOOLEAN:
      return "bool";
    case Keyword::CHAR:
      return "char";
    case Keyword::VOID:
      return "void";
    case Keyword::VAR:
      return "var";
    case Keyword::STATIC:
      return "static";
    case Keyword::FIELD:
      return "field";
    case Keyword::LET:
      return "let";
    case Keyword::DO:
      return "do";
    case Keyword::IF:
      return "if";
    case Keyword::ELSE:
      return "else";
    case Keyword::WHILE:
      return "while";
    case Keyword::RETURN:
      return "return";
    case Keyword::TRUE:
      return "true";
    case Keyword::FALSE:
      return "false";
    case Keyword::NULL_VAL:
      return "null";
    case Keyword::THIS:
      return "this";
    default:
      return "unknown";
  }
}

static Keyword GetKeywordFromString(const std::string keyword_str) {
  auto keyword_pair = keyword_map.find(keyword_str);
  if (keyword_pair == keyword_map.end()) {
    return Keyword::UNKNOWN;
  }
  return keyword_pair->second;
}

static bool IsKeyword(const std::string token_str) {
  return (keyword_map.find(token_str) != keyword_map.end());
}

#endif  // KEYWORD_H
