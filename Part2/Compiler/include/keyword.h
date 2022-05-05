// An Enum representing the collection of keywords in the Jack language.
#ifndef KEYWORD_H
#define KEYWORD_H

#include <unordered_map>
#include <string>

class Keyword {
public:
  enum class Type {
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

  static std::string KeywordToString(Keyword::Type k);

  static Keyword::Type GetKeywordFromString(const std::string keyword_str);

  static bool IsKeyword(const std::string token_str);

  static bool IsPrimitiveType(const Keyword::Type k);

  static bool IsKeywordConstant(const Keyword::Type k);
private:
  static std::unordered_map<std::string, Keyword::Type> const keyword_map_;
};

#endif  // KEYWORD_H
