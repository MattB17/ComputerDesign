// An Enum representing the collection of possible VM instructions.
#ifndef OPERATION_H
#define OPERATION_H

#include <unordered_map>
#include <string>

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
  UNKNOWN = 9;
}

static std::unordered_map<std::string, Operation> const operation_map =
  {
    {"add", Operation::ARITHMETIC},
    {"sub", Operation::ARITHMETIC},
    {"neg", Operation::ARITHMETIC},
    {"eq", Operation::ARITHMETIC},
    {"gt", Operation::ARITHMETIC},
    {"lt", Operation::ARITHMETIC},
    {"and", Operation::ARITHMETIC},
    {"or", Operation::ARITHMETIC},
    {"not", Operation::ARITHMETIC},
    {"push", Operation::PUSH},
    {"pop", Operation::POP},
    {"label", Operation::LABEL},
    {"goto", Operation::GOTO},
    {"if", Operation::IF},
    {"function", Operation::FUNCTION},
    {"call", Operation::CALL},
    {"return", Operation::RETURN}
  };

static Operation GetOperationFromString(std::string op_str) {
  auto op_pair = operation_map.find(op_str);
  if (op_pair == operation_map.end()) {
    return Operation::UNKNOWN;
  }
  return op_pair->second;
}

#endif  // OPERATION_H
