// An enum storing the kinds of variables that can be declared in Jack.

enum class Segment {
  FIELD = 0,
  STATIC = 1,
  ARGUMENT = 2,
  LOCAL = 3,
  UNKNOWN = 4
};
