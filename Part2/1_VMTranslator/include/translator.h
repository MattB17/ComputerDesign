// A class to translate VM Commands into assembly commands and write the
// output to a file stream.
#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <string>
#include <sstream>

class Translator {
public:
  Translator(std::string static_segment);
  Translator(const Translator&) = delete;
  Translator &operator=(const Translator&) = delete;
  Translator(Translator&&) = delete;
  Translator &operator=(Translator&&) = delete;
  ~Translator() {}

  // translates the VM arithmetic command given by `operation`.
  std::string translateArithmeticOperation(std::string operation);

  // translates the VM push operation of the form `push segment i`.
  std::string translatePushOperation(std::string segment, int i);

  // translates the VM pop operation of the form `pop segment i`.
  std::string translatePopOperation(std::string segment, int i);

  // translates the VM label operation of the form `label label_str`.
  std::string translateLabelOperation(std::string label_str);

  // translates the VM goto operation of the form `goto label_str`.
  std::string translateGoToOperation(std::string label_str);

  // translates the VM if-goto operation of the form `if-goto label_str`.
  std::string translateIfGoToOperation(std::string label_str);

  // translates the VM function operation of the form
  // `function functionName nVars`.
  std::string translateFunctionOperation(std::string function_name, int n_vars);

private:
  // translates a VM combination command. One of `add`, `sub`, `and`, or `or`.
  std::string translateCombination(std::string comparison_expression);

  // translates a VM negation command. One of `neg` or `not`.
  std::string translateNegation(std::string negation_expression);

  // translates a VM comparison command. One of `eq`, `lt`, or `gt`.
  std::string translateComparison(std::string comparison_expression);

  // translates the VM instruction `push constant i`.
  std::string pushConstant(int i);

  // translates the VM instruction `push segment i` where segment is
  // one of `local`, `argument`, `this`, or `that`.
  std::string pushSegment(int i);

  // translates the VM instruction `push temp i`.
  std::string pushTemp(int i);

  // pushes the current memory contents to the stack and increments the
  // stack pointer.
  std::string pushMemoryContentsToStack();

  // adds `offset` to the current address pointed to by the D register and
  // pushes the contents of that address to the stack, while incrementing the
  // stack pointer.
  std::string addOffsetAndPushToStack(int offset);

  // translates the VM instruction `pop segment i` where segment is
  // one of `local`, `argument`, `this`, or `that`.
  std::string popSegment(int i);

  // translates the VM instruction `pop temp i`.
  std::string popTemp(int i);

  // translates the VM instruction `pop static i`.
  std::string popStatic(int i);

  // translates the VM instruction `push pointer i`
  std::string popPointer(int i);

  // adds `offset` to the current address pointed to by the D register and
  // pops the head of the stack to that address, while decrementing the
  // stack pointer.
  std::string addOffsetAndPopFromStack(int offset);

  // refreshes the output stream.
  void refreshOutputStream();

  // Sets the value of the `i` register based on the value of `i`. That
  // is the address referenced by the `pointer i` instruction is determined.
  void setAddressFromPointer(int i);

  // the assembly commands for incrementing the stack pointer
  void stackPointerIncrementInstruction();

  // the assembly commands for decrementing the stack pointer
  void stackPointerDecrementInstruction();

  // the assembly commands to assign the top of the stack to the A register
  // and to decrement the stack pointer.
  void decrementStackPointerAndAssignToA();

  // the assembly commands to assign the top of the stack to the A register
  // and to decrement the stack pointer.
  void decrementStackPointerAndAssignToD();

  // the assembly command to add the label `label_str`.
  void addLabel(std::string label_str);

  // adds the assembly commands to execute the instruction `push constant i`.
  void addPushConstantInstruction(int i);

  int label_idx_;

  std::string static_segment_;

  std::stringstream out_stream_;
};

#endif  // TRANSLATOR_H
