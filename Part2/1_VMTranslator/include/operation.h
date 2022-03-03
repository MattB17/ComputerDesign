// An Enum representing the collection of possible VM instructions.
#ifndef OPERATION_H
#define OPERATION_H

enum Operation {
  // The set of possible VM operations
  ARITHMETIC = 0;
  PUSH = 1;
  POP = 2;
  LABEL = 3;
  GOTO = 4;
  IF = 5;
  FUNCTION = 6;
  CALL = 7
  RETURN = 8;
}

#endif  // OPERATION_H
