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

  // translates the VM instruction `pop segment i` where segment is
  // one of `local`, `argument`, `this`, or `that`.
  std::string popSegment(std::string segment, int i);

  // translates the VM instruction `pop temp i`.
  std::string popTemp(int i);

  // translates the VM instruction `pop static i`.
  std::string popStatic(int i);

  // translates the VM instruction `push pointer i`
  std::string popPointer(int i);

  std::string translatePushOperation(std::string segment, int i);

  // translates the VM arithmetic command given by `operation`.
  std::string translateArithmeticOperation(std::string operation);

private:
  // translates a VM combination command. One of `add`, `sub`, `and`, or `or`.
  std::string translateCombination(
    std::stringstream& out_stream, std::string comparison_expression);

  // translates a VM negation command. One of `neg` or `not`.
  std::string translateNegation(
    std::stringstream& out_stream, std::string negation_expression);

  // translates a VM comparison command. One of `eq`, `lt`, or `gt`.
  std::string translateComparison(
    std::stringstream& out_stream, std::string comparison_expression);

  // translates the VM instruction `push constant i`.
  std::string pushConstant(std::stringstream& out_stream, int i);

  // translates the VM instruction `push segment i` where segment is
  // one of `local`, `argument`, `this`, or `that`.
  std::string pushSegment(std::stringstream& out_stream, int i);

  // translates the VM instruction `push temp i`.
  std::string pushTemp(std::stringstream& out_stream, int i);

  // pushes the current memory contents to the stack and increments the
  // stack pointer.
  std::string pushMemoryContentsToStack(std::stringstream& out_stream);

  // adds `offset` to the current address pointed to by the A register and
  // pushes the contents of that address to the stack, while incrementing the
  // stack pointer.
  std::string addOffsetAndPushToStack(
    std::stringstream& out_stream, int offset);

  // the assembly commands for incrementing the stack pointer
  static std::string stackPointerIncrementInstruction();

  // the assembly commands for decrementing the stack pointer
  static std::string stackPointerDecrementInstruction();

  static std::string decrementStackPointerAndAssignToA();

  int label_idx_;

  std::string static_segment_;
};

#endif  // TRANSLATOR_H
