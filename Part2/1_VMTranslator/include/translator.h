// A class to translate VM Commands into assembly commands and write the
// output to a file stream.
#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <string>
#include <sstream>

class Translator {
public:
  Translator();
  Translator(const Translator&) = delete;
  Translator &operator=(const Translator&) = delete;
  Translator(Translator&&) = delete;
  Translator &operator=(Translator&&) = delete;
  ~Translator() {}

  void setStaticSegmentName(std::string static_segment) {
    static_segment_ = static_segment;
  }

  // translates the system init operation into assembly code.
  std::string translateInitOperation();

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
  // `function function_name n_vars`.
  std::string translateFunctionOperation(
    std::string function_name, int n_vars);

  // translates the VM return operation of the form `return`.
  std::string translateReturnOperation();

  // translates the VM call operation of the form `call function_name n_args`.
  std::string translateCallOperation(std::string function_name, int n_args);

private:
  // translates a VM combination command. One of `add`, `sub`, `and`, or `or`.
  void translateCombination(std::string comparison_expression);

  // translates a VM negation command. One of `neg` or `not`.
  void translateNegation(std::string negation_expression);

  // translates a VM comparison command. One of `eq`, `lt`, or `gt`.
  void translateComparison(std::string comparison_expression);

  // translates the VM instruction `push constant i`.
  void pushConstant(int i);

  // translates the VM instruction `push segment i` where segment is
  // one of `local`, `argument`, `this`, or `that`.
  void pushSegment(int i);

  // translates the VM instruction `push temp i`.
  void pushTemp(int i);

  // adds `offset` to the current address pointed to by the D register and
  // pushes the contents of that address to the stack, while incrementing the
  // stack pointer.
  void addOffsetAndPushToStack(int offset);

  // translates the VM instruction `pop segment i` where segment is
  // one of `local`, `argument`, `this`, or `that`.
  void popSegment(int i);

  // translates the VM instruction `pop temp i`.
  void popTemp(int i);

  // translates the VM instruction `pop static i`.
  void popStatic(int i);

  // translates the VM instruction `push pointer i`
  void popPointer(int i);

  // adds `offset` to the current address pointed to by the D register and
  // pops the head of the stack to that address, while decrementing the
  // stack pointer.
  void addOffsetAndPopFromStack(int offset);

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

  // decrements the value stored in the register `register_name` and assigns
  // the resulting value to the memory location pointed to by `segment_name`.
  void decrementRegisterAndAssignToSegment(
    std::string register_name, std::string segment_name);

  // the assembly command to create the label `label_str`.
  void createLabel(std::string label_str);

  // the assembly command `@label_str`.
  void atLabelCommand(std::string label_str);

  // adds the label string for `label_str`.
  void addLabelString(std::string label_str);

  // adds the return address
  void addReturnAddress();

  // adds the assembly commands to push the value stored in register A onto the
  // stack.
  void pushValueInRegisterA();

  // adds the assembly commands to push the value stored in register M onto the
  // stack.
  void pushValueInRegisterM();

  // adds the assembly commands to push the value stored in register D onto the
  // stack.
  void pushValueInRegisterD();

  // adds the assembly commands to save the state of the current function and
  // jump to the function `function_name` taking `n_args`.
  void saveCurrStateAndJumpToFunction(std::string function_name, int n_args);

  int label_idx_;
  std::string static_segment_;
  // identifies the name of the current function. An empty string indicates
  // that the translation is currently being done outside of any functions.
  std::string curr_function_;
  // indicates the number of call commands executed inside the current
  // function.
  int func_calls_;
  std::stringstream out_stream_;
};

#endif  // TRANSLATOR_H
