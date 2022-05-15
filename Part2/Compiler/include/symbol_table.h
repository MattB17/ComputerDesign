// A class to store the variables in a particular scope.
#include <string>

#include "segment.h"

struct varData {
  std::string type;
  Segment segment;
  int segment_offset;
};
