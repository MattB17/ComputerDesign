// A class to translate VM Commands into assembly commands and write the
// output to a file stream.
#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <string>

class Translator {
public:
  Translator(std::string static_segment);
  Translator(const Translator&) = delete;
  Translator &operator=(const Translator&) = delete;
  Translator(Translator&&) = delete;
  Translator &operator=(Translator&&) = delete;
  ~Translator() {}

  // translates the VM instruction `push constant i`.
  std::string pushConstant(int i);

  // translates the VM instruction `push segment i` where segment is
  // one of `local`, `argument`, `this`, or `that`.
  std::string pushSegment(std::string segment, int i);

  // translates the VM instruction `push temp i`.
  std::string pushTemp(int i);

  // translates the VM instruction `push static i`.
  std::string pushStatic(int i);

  // translates the VM instruction `push pointer i`.
  std::string pushPointer(int i);

  // translates the VM instruction `pop segment i` where segment is
  // one of `local`, `argument`, `this`, or `that`.
  std::string popSegment(std::string segment, int i);

  // translates the VM instruction `pop temp i`.
  std::string popTemp(int i);

  // translates the VM instruction `pop static i`.
  std::string popStatic(int i);

  // translates the VM instruction `push pointer i`
  std::string popPointer(int i);

  // translates the VM arithmetic command given by `operation`.
  std::string translateArithmeticOperation(std::string operation);

private:
  // translates a VM combination command. One of `add`, `sub`, `and`, or `or`.
  std::string translateCombination(std::string comparison_expression);

  // translates a VM negation command. One of `neg` or `not`.
  std::string translateNegation(std::string negation_expression);

  // translates a VM comparison command. One of `eq`, `lt`, or `gt`.
  std::string translateComparison(std::string comparison_expression);

  // the assembly commands for incrementing the stack pointer
  static std::string stackPointerIncrementInstruction();

  // the assembly commands for decrementing the stack pointer
  static std::string stackPointerDecrementInstruction();

  int label_idx_;

  std::string static_segment_;
};

#endif  // TRANSLATOR_H
