#include "keyword.h"

std::unordered_map<std::string, Keyword::Type> const Keyword::keyword_map_ =
  {
    {"class", Type::CLASS},
    {"method", Type::METHOD},
    {"function", Type::FUNCTION},
    {"constructor", Type::CONSTRUCTOR},
    {"int", Type::INT},
    {"bool", Type::BOOLEAN},
    {"boolean", Type::BOOLEAN},
    {"char", Type::CHAR},
    {"void", Type::VOID},
    {"var", Type::VAR},
    {"static", Type::STATIC},
    {"field", Type::FIELD},
    {"let", Type::LET},
    {"do", Type::DO},
    {"if", Type::IF},
    {"else", Type::ELSE},
    {"while", Type::WHILE},
    {"return", Type::RETURN},
    {"true", Type::TRUE},
    {"false", Type::FALSE},
    {"null", Type::NULL_VAL},
    {"this", Type::THIS}
  };

std::string Keyword::KeywordToString(Keyword::Type k) {
  switch (k) {
    case Type::CLASS:
      return "class";
    case Type::METHOD:
      return "method";
    case Type::FUNCTION:
      return "function";
    case Type::CONSTRUCTOR:
      return "constructor";
    case Type::INT:
      return "int";
    case Type::BOOLEAN:
      return "bool";
    case Type::CHAR:
      return "char";
    case Type::VOID:
      return "void";
    case Type::VAR:
      return "var";
    case Type::STATIC:
      return "static";
    case Type::FIELD:
      return "field";
    case Type::LET:
      return "let";
    case Type::DO:
      return "do";
    case Type::IF:
      return "if";
    case Type::ELSE:
      return "else";
    case Type::WHILE:
      return "while";
    case Type::RETURN:
      return "return";
    case Type::TRUE:
      return "true";
    case Type::FALSE:
      return "false";
    case Type::NULL_VAL:
      return "null";
    case Type::THIS:
      return "this";
    default:
      return "unknown";
  }
}

Keyword::Type Keyword::GetKeywordFromString(const std::string keyword_str) {
  auto keyword_pair = keyword_map_.find(keyword_str);
  if (keyword_pair == keyword_map_.end()) {
    return Type::UNKNOWN;
  }
  return keyword_pair->second;
}

bool Keyword::IsKeyword(const std::string token_str) {
  return (keyword_map_.find(token_str) != keyword_map_.end());
}

bool Keyword::IsPrimitiveType(const Keyword::Type k) {
  return ((k == Type::INT) || (k == Type::CHAR) || (k == Type::BOOLEAN));
}

bool Keyword::IsKeywordConstant(const Keyword::Type k) {
  return ((k == Type::TRUE) ||
          (k == Type::FALSE) ||
          (k == Type::NULL_VAL) ||
          (k == Type::THIS));
}
