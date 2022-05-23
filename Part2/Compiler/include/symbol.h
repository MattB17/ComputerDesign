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

static std::unordered_set<char> const unaryOps = {'-', '~'};

static std::unordered_set<char> const binaryOps = {
  '+', '-', '*', '/', '&', '|', '<', '>', '='
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

static bool IsSymbol(const char curr_char) {
  return ((otherSymbols.find(curr_char) != otherSymbols.end()) ||
          (binaryOps.find(curr_char) != binaryOps.end()) ||
          (unaryOps.find(curr_char) != unaryOps.end()));
}

static bool IsUnaryOp(const char curr_char) {
  return (unaryOps.find(curr_char) != unaryOps.end());
}

static bool IsBinaryOp(const char curr_char) {
  return (binaryOps.find(curr_char) != binaryOps.end());
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
