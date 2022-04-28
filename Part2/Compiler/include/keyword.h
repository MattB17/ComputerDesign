// An Enum representing the collection of keywords in the Jack language.
#ifndef KEYWORD_H
#define KEYWORD_H

#include <unordered_map>
#include <string>

enum Keyword {
  // The set of possible keywords
  kCLASS = 0,
  kMETHOD = 1,
  kFUNCTION = 2,
  kCONSTRUCTOR = 3,
  kINT = 4,
  kBOOLEAN = 5,
  kCHAR = 6,
  kVOID = 7,
  kVAR = 8,
  kSTATIC = 9,
  kFIELD = 10,
  kLET = 11,
  kDO = 12,
  kIF = 13,
  kELSE = 14,
  kWHILE = 15,
  kRETURN = 16,
  kTRUE = 17,
  kFALSE = 18,
  kNULL = 19,
  kTHIS = 20,
  kUNKNOWN = 21
};

static std::unordered_map<std::string, Keyword> const keyword_map =
  {
    {"class", Keyword::kCLASS},
    {"method", Keyword::kMETHOD},
    {"function", Keyword::kFUNCTION},
    {"constructor", Keyword::kCONSTRUCTOR},
    {"int", Keyword::kINT},
    {"bool", Keyword::kBOOLEAN},
    {"char", Keyword::kCHAR},
    {"void", Keyword::kVOID},
    {"var", Keyword::kVAR},
    {"static", Keyword::kSTATIC},
    {"field", Keyword::kFIELD},
    {"let", Keyword::kLET},
    {"do", Keyword::kDO},
    {"if", Keyword::kIF},
    {"else", Keyword::kELSE},
    {"while", Keyword::kWHILE},
    {"return", Keyword::kRETURN},
    {"true", Keyword::kTRUE},
    {"false", Keyword::kFALSE},
    {"null", Keyword::kNULL},
    {"this", Keyword::kTHIS}
  };

static Keyword GetKeywordFromString(const std::string keyword_str) {
  auto keyword_pair = keyword_map.find(keyword_str);
  if (keyword_pair == keyword_map.end()) {
    return Keyword::kUNKNOWN;
  }
  return keyword_pair->second;
}

static bool IsKeyword(const std::string token_str) {
  return (keyword_map.find(token_str) != keyword_map.end());
}

#endif  // KEYWORD_H
