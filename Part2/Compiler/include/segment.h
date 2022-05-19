// An enum storing the kinds of variables that can be declared in Jack.
#ifndef SEGMENT_H
#define SEGMENT_H

enum class Segment {
  FIELD = 0,
  STATIC = 1,
  ARGUMENT = 2,
  LOCAL = 3,
  NONE = 4
};

#endif  // SEGMENT_H
