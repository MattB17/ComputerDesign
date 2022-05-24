// Contains a collection of static methods for dealing with symbols.
#ifndef SYMBOL_H
#define SYMBOL_H

#include <unordered_map>
#include <unordered_set>

enum class OpCommand {
  ADD = 0,
  SUB = 1,
  NEG = 2,
  EQ = 3,
  GT = 4,
  LT = 5,
  AND = 6,
  OR = 7,
  NOT = 8,
  UNKNOWN = 9
};

static std::unordered_map<char, OpCommand> const unaryOpsMap = {
  {'-', OpCommand::NEG},
  {'~', OpCommand::NOT}
};

static std::unordered_map<char, OpCommand> const simpleBinaryOpsMap = {
  {'+', OpCommand::ADD},
  {'-', OpCommand::SUB},
  {'&', OpCommand::AND},
  {'|', OpCommand::OR},
  {'<', OpCommand::LT},
  {'>', OpCommand::GT},
  {'=', OpCommand::EQ}
};

static std::unordered_map<char, std::string> const mathOpsMap = {
  {'*', "Math.multiply"},
  {'/', "Math.divide"}
};

static std::unordered_set<char> const otherSymbols = {
  '{', '}', '(', ')', '[', ']', '.', ',', ';'
};

static std::unordered_map<char, std::string> const xmlSymbolMap = {
  {'<', "&lt;"},
  {'>', "&gt;"},
  {'"', "&quot;"},
  {'&', "&amp;"}
};

static bool IsUnaryOp(const char curr_char) {
  return (unaryOpsMap.find(curr_char) != unaryOpsMap.end());
}

static bool IsSimpleBinaryOp(const char curr_char) {
  return (simpleBinaryOpsMap.find(curr_char) != simpleBinaryOpsMap.end());
}

static bool IsMathOp(const char curr_char) {
  return (mathOpsMap.find(curr_char) != mathOpsMap.end());
}

static bool IsSymbol(const char curr_char) {
  return ((otherSymbols.find(curr_char) != otherSymbols.end()) ||
          IsUnaryOp(curr_char) ||
          IsSimpleBinaryOp(curr_char) ||
          IsMathOp(curr_char));
}

static OpCommand GetUnaryOpCommand(const char curr_char) {
  return unaryOpsMap.find(curr_char)->second;
}

static OpCommand GetSimpleBinaryOpCommand(const char curr_char) {
  return simpleBinaryOpsMap.find(curr_char)->second;
}

static std::string GetMathOpFunction(const char curr_char) {
  return mathOpsMap.find(curr_char)->second;
}

static std::string SymbolToString(const char curr_char) {
  auto xml_pair = xmlSymbolMap.find(curr_char);
  if (xml_pair != xmlSymbolMap.end()) {
    return xml_pair->second;
  }
  return std::string(1, curr_char);
}

static std::string OpCommandToString(OpCommand op_command) {
  switch (op_command) {
    case OpCommand::ADD:
      return "add";
    case OpCommand::SUB:
      return "sub";
    case OpCommand::NEG:
      return "neg";
    case OpCommand::EQ:
      return "eq";
    case OpCommand::GT:
      return "gt";
    case OpCommand::LT:
      return "lt";
    case OpCommand::AND:
      return "and";
    case OpCommand::OR:
      return "or";
    case OpCommand::NOT:
      return "not";
    default:
      return "unknown";
  }
}

#endif  // SYMBOL_H
