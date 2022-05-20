// An enum storing the kinds of variables that can be declared in Jack.
#ifndef SEGMENT_H
#define SEGMENT_H

#include <string>

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

#endif  // SEGMENT_H
