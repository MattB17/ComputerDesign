// An enum storing the kinds of variables that can be declared in Jack.
#ifndef SEGMENT_H
#define SEGMENT_H

#include <string>

#include "keyword.h"

enum class Segment {
  CONSTANT = 0,
  ARGUMENT = 1,
  LOCAL = 2,
  STATIC = 3,
  THIS = 4,
  THAT = 5,
  POINTER = 6,
  TEMP = 7,
  UNKNOWN = 8
};

static std::string SegmentToString(Segment segment) {
  switch (segment) {
    case Segment::CONSTANT:
      return "constant";
    case Segment::ARGUMENT:
      return "argument";
    case Segment::LOCAL:
      return "local";
    case Segment::STATIC:
      return "static";
    case Segment::THIS:
      return "this";
    case Segment::THAT:
      return "that";
    case Segment::POINTER:
      return "pointer";
    case Segment::TEMP:
      return "temp";
    default:
      return "unknown";
  }
}

static Segment GetSegmentFromClassVarKeyword(Keyword::Type var_keyword) {
  if (var_keyword == Keyword::Type::FIELD) {
    return Segment::THIS;
  }
  return Segment::STATIC;
}

#endif  // SEGMENT_H
