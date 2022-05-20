// An enum storing the kinds of variables that can be declared in Jack.
#ifndef SEGMENT_H
#define SEGMENT_H

#include <string>

#include "keyword.h"

enum class Segment {
  FIELD = 0,
  STATIC = 1,
  ARGUMENT = 2,
  LOCAL = 3,
  NONE = 4
};

static std::string SegmentToString(Segment segment) {
  switch (segment) {
    case Segment::FIELD:
      return "field";
    case Segment::STATIC:
      return "static";
    case Segment::ARGUMENT:
      return "argument";
    case Segment::LOCAL:
      return "local";
    default:
      return "none";
  }
}

static Segment GetSegmentFromClassVarKeyword(Keyword::Type var_keyword) {
  if (var_keyword == Keyword::Type::FIELD) {
    return Segment::FIELD;
  }
  return Segment::STATIC;
}

#endif  // SEGMENT_H
