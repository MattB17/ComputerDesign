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

static Operation GetOperationFromString(const std::string op_str) {
  auto op_pair = operation_map.find(op_str);
  if (op_pair == operation_map.end()) {
    return Operation::UNKNOWN;
  }
  return op_pair->second;
}

static bool IsOperationWithNoArguments(const Operation vm_op) {
  return (vm_op == Operation::RETURN || vm_op == Operation::UNKNOWN);
}

static bool IsOperationWithTwoArguments(const Operation vm_op) {
  return (vm_op == Operation::PUSH ||
          vm_op == Operation::POP ||
          vm_op == Operation::FUNCTION ||
          vm_op == Operation::CALL);
}

#endif  // OPERATION_H
