#include "translator.h"

#include <sstream>

Translator::Translator(std::string static_segment_)
  : label_idx_(0),  static_segment_(static_segment_),
    curr_function_(""), func_calls_(0) {}

std::string Translator::translateArithmeticOperation(std::string operation) {
  refreshOutputStream();
  if (operation.compare("add") == 0) {
    // D = x + y
    translateCombination("D=D+M");
  } else if (operation.compare("sub") == 0) {
    // D = x - y
    translateCombination("D=M-D");
  } else if (operation.compare("and") == 0) {
    // D = x & y
    translateCombination("D=D&M");
  } else if (operation.compare("or") == 0) {
    // D = x | y
    translateCombination("D=D|M");
  } else if (operation.compare("neg") == 0) {
    // M = -x
    translateNegation("M=-M");
  } else if (operation.compare("not") == 0) {
    // M = !x
    translateNegation("M=!M");
  } else if (operation.compare("eq") == 0) {
    // D = x - y, jump if D == 0
    translateComparison("D;JEQ");
  } else if (operation.compare("lt") == 0) {
    // D = x - y, jump if D < 0
    translateComparison("D;JLT");
  } else if (operation.compare("gt") == 0) {
    // D = x - y, jump if D > 0
    translateComparison("D;JGT");
  } else {
    return "";
  }
  return out_stream_.str();
}

std::string Translator::translatePushOperation(std::string segment, int i) {
  refreshOutputStream();
  if (segment.compare("constant") == 0) {
    pushConstant(i);
  } else if (segment.compare("local") == 0) {
    out_stream_ << "@LCL\n";
    pushSegment(i);
  } else if (segment.compare("argument") == 0) {
    out_stream_ << "@ARG\n";
    pushSegment(i);
  } else if (segment.compare("this") == 0) {
    out_stream_ << "@THIS\n";
    pushSegment(i);
  } else if (segment.compare("that") == 0) {
    out_stream_ << "@THAT\n";
    pushSegment(i);
  } else if (segment.compare("temp") == 0) {
    out_stream_ << "@5\n";
    pushTemp(i);
  } else if (segment.compare("static") == 0) {
    out_stream_ << "@" << static_segment_ << "." << i << "\n";
    pushValueInRegisterM();
  } else if (segment.compare("pointer") == 0) {
    setAddressFromPointer(i);
    pushValueInRegisterM();
  } else {
    return "";
  }
  return out_stream_.str();
}

std::string Translator::translatePopOperation(std::string segment, int i) {
  refreshOutputStream();
  if (segment.compare("local") == 0) {
    out_stream_ << "@LCL\n";
    popSegment(i);
  } else if (segment.compare("argument") == 0) {
    out_stream_ << "@ARG\n";
    popSegment(i);
  } else if (segment.compare("this") == 0) {
    out_stream_ << "@THIS\n";
    popSegment(i);
  } else if (segment.compare("that") == 0) {
    out_stream_ << "@THAT\n";
    popSegment(i);
  } else if (segment.compare("temp") == 0) {
    out_stream_ << "@5\n";
    popTemp(i);
  } else if (segment.compare("static") == 0) {
    popStatic(i);
  } else if (segment.compare("pointer") == 0) {
    popPointer(i);
  } else {
    return "";
  }
  return out_stream_.str();
}

std::string Translator::translateLabelOperation(std::string label_str) {
  refreshOutputStream();
  createLabel(label_str);
  return out_stream_.str();
}

std::string Translator::translateGoToOperation(std::string label_str) {
  refreshOutputStream();
  atLabelCommand(label_str);
  out_stream_ << "0;JMP\n";
  return out_stream_.str();
}

std::string Translator::translateIfGoToOperation(std::string label_str) {
  refreshOutputStream();
  decrementStackPointerAndAssignToD();
  atLabelCommand(label_str);
  out_stream_ << "D;JNE\n";
  return out_stream_.str();
}

std::string Translator::translateFunctionOperation(
  std::string function_name, int n_vars) {
  // clearing state when entering function
  curr_function_ = "";
  func_calls_ = 0;

  refreshOutputStream();

  createLabel(function_name);

  curr_function_ = function_name;

  for (int i = 0; i < n_vars; i++) {
    pushConstant(0);
  }

  return out_stream_.str();
}

std::string Translator::translateReturnOperation() {
  refreshOutputStream();

  // D = *LCL
  out_stream_ << "@LCL\n";
  out_stream_ << "D=M\n";

  // *R13 = D (R13 = LCL representing endFrame)
  out_stream_ << "@R13\n";
  out_stream_ << "M=D\n";

  // D = *R13 - 5
  out_stream_ << "@5\n";
  out_stream_ << "A=D-A\n";
  out_stream_ << "D=M\n";

  // *R14 = D
  out_stream_ << "@R14\n";
  out_stream_ << "M=D\n";

  // RAM[*ARG] = pop()
  out_stream_ << "@ARG\n";
  out_stream_ << "D=M\n";
  addOffsetAndPopFromStack(/*offset=*/0);

  // D = *ARG
  out_stream_ << "@ARG\n";
  out_stream_ << "D=M\n";

  // *SP = D + 1 (*SP = *ARG + 1 because D = *ARG)
  out_stream_ << "@SP\n";
  out_stream_ << "M=D+1\n";

  // *THAT = *R13 - 1 (R13 stores endFrame). Update R13 to store endFrame - 1.
  decrementRegisterAndAssignToSegment("R13", "THAT");

  // *THIS = *R13 - 1 (R13 stores endFrame - 1).
  // Update R13 to store endFrame - 2.
  decrementRegisterAndAssignToSegment("R13", "THIS");

  // *ARG = *R13 - 1 (R13 stores endFrame - 2).
  // Update R13 to store endFrame - 3.
  decrementRegisterAndAssignToSegment("R13", "ARG");

  // *LCL = *R13 - 1 (R13 stores endFrame - 3).
  // Update R13 to store endFrame - 4.
  decrementRegisterAndAssignToSegment("R13", "LCL");

  // goto *R14 (R14 stores retAddr)
  out_stream_ << "@R14\n";
  out_stream_ << "A=M\n";
  out_stream_ << "0;JMP\n";

  return out_stream_.str();
}

std::string Translator::translateCallOperation(
  std::string function_name, int n_args) {
  refreshOutputStream();

  // @returnAddress
  out_stream_ << "@";
  addReturnAddress();
  out_stream_ << "\n";

  pushValueInRegisterA();

  // push LCL
  out_stream_ << "@LCL\n";
  pushValueInRegisterM();

  // push ARG
  out_stream_ << "@ARG\n";
  pushValueInRegisterM();

  // push THIS
  out_stream_ << "@THIS\n";
  pushValueInRegisterM();

  // push THAT
  out_stream_ << "@THAT\n";
  pushValueInRegisterM();

  // D = *SP
  out_stream_ << "@SP\n";
  out_stream_ << "D=M\n";

  // *LCL = D (*LCL = *SP as D = *SP)
  out_stream_ << "@LCL\n";
  out_stream_ << "M=D\n";

  // D = D - (5 + n_args) (D = *SP - 5 - n_args as D = *SP)
  out_stream_ << "@" << (5 + n_args) << "\n";
  out_stream_ << "D=D-A\n";

  // *ARG = D (*ARG = *SP - 5 - n_args as D = *SP - 5 - n_args)
  out_stream_ << "@ARG\n";
  out_stream_ << "M=D\n";

  // goto function_name
  out_stream_ << "@" << function_name << "\n";
  out_stream_ << "0;JMP\n";

  // (returnAddress)
  out_stream_ << "(";
  addReturnAddress();
  out_stream_ << ")\n";

  func_calls_++;
  return out_stream_.str();
}

/* *****************
 * PRIVATE MEMBERS
 * ****************/

void Translator::translateCombination(std::string combination_expression) {
  // D = *(SP-1) - this is the variable y
  out_stream_ << "@SP\n";
  out_stream_ << "A=M-1\n";
  out_stream_ << "D=M\n";

  // A = *(SP-2) - this is now the address of x
  out_stream_ << "A=A-1\n";
  // M stores the value of X and D stores the value of y.
  // So add the line with the combination expression
  out_stream_ << combination_expression << "\n";

  // *(SP-2) = D
  out_stream_ << "M=D\n";

  stackPointerDecrementInstruction();
}

void Translator::translateNegation(std::string negation_expression) {
  // *(SP-1) = -(*(SP-1)) - that is just directly access the variable
  // and negate, no need to pop it off and push it back
  out_stream_ << "@SP\n";
  out_stream_ << "A=M-1\n";
  // M stores the variable x so write out the negation expression
  out_stream_ << negation_expression << "\n";
}

void Translator::translateComparison(std::string comparison_expression) {
  // D = *(SP-1) - this is the variable y
  out_stream_ << "@SP\n";
  out_stream_ << "A=M-1\n";
  out_stream_ << "D=M\n";

  // A = *(SP-2) - this is now the address of x
  out_stream_ << "A=A-1\n";
  // D = x - y (D already stores the value of y)
  out_stream_ << "D=M-D\n";

  // put value of true in *(SP-2)
  out_stream_ << "M=-1\n";

  // if D = x - y and we already put true in the stack position.
  // So if the comparison evaluates to true jump to cleanup
  out_stream_ << "@CLEANUP" << label_idx_ << "\n";
  out_stream_ << comparison_expression << "\n";

  // otherwise, flip true to false in the stack position
  out_stream_ << "@SP\n";
  out_stream_ << "A=M-1\n";
  out_stream_ << "A=A-1\n";
  out_stream_ << "M=M+1\n";

  // SP--;
  out_stream_ << "(CLEANUP" << label_idx_ << ")\n";
  stackPointerDecrementInstruction();

  label_idx_++;
}

void Translator::pushConstant(int i) {
  // A = i
  out_stream_ << "@" << i << "\n";

  pushValueInRegisterA();
}

void Translator::pushSegment(int i) {
  // D = RAM[@segment] (A already points to @segment)
  out_stream_ << "D=M\n";

  addOffsetAndPushToStack(/*offset=*/i);
}

void Translator::pushTemp(int i) {
  // D = 5 (A = @temp, that is A = 5)
  out_stream_ << "D=A\n";

  addOffsetAndPushToStack(/*offset=*/i);
}

void Translator::addOffsetAndPushToStack(int offset) {
  // A = D + offset (where D = RAM[@segment] so A = RAM[@segment] + offset).
  out_stream_ << "@" << offset << "\n";
  out_stream_ << "A=D+A\n";

  pushValueInRegisterM();
}

void Translator::popSegment(int i) {
  // D = RAM[@segment] (A already has the value @segment)
  out_stream_ << "D=M\n";

  addOffsetAndPopFromStack(/*offset=*/i);
}

void Translator::popTemp(int i) {
  // D = 5 (A already stores 5, the start of the temp segment)
  out_stream_ << "D=A\n";

  addOffsetAndPopFromStack(/*offset=*/i);
}

void Translator::popStatic(int i) {
  decrementStackPointerAndAssignToD();

  // @Foo.i = D where `Foo` is the name of the static segment
  out_stream_ << "@" << static_segment_ << "." << i << "\n";
  out_stream_ << "M=D\n";
}

void Translator::popPointer(int i) {
  decrementStackPointerAndAssignToD();

  // @Ptr = D where `Ptr` is either `THIS` or `THAT` depending on `i`.
  setAddressFromPointer(i);
  out_stream_ << "M=D\n";
}

void Translator::addOffsetAndPopFromStack(int offset) {
  // D = D + i (where D = RAM[@segment], so D = RAM[@segment] + i)
  out_stream_ << "@" << offset << "\n";
  out_stream_ << "D=D+A\n";

  decrementStackPointerAndAssignToA();

  // add this element to D so now D stores `@segment + i + val` where
  // `val` is the value popped off the stack
  out_stream_ << "D=D+M\n";

  // update A to `@segment + i`
  out_stream_ << "A=D-M\n";

  // update RAM[@segment + i] to `val` which is D - A
  out_stream_ << "M=D-A\n";
}

void Translator::refreshOutputStream() {
  out_stream_.clear();
  out_stream_.str(std::string());
}

void Translator::setAddressFromPointer(int i) {
  if (i == 0) {
    out_stream_ << "@THIS\n";
  } else {
    out_stream_ << "@THAT\n";
  }
}

void Translator::stackPointerIncrementInstruction() {
  out_stream_ << "@SP\n";
  out_stream_ << "M=M+1\n";
}

void Translator::stackPointerDecrementInstruction() {
  out_stream_ << "@SP\n";
  out_stream_ << "M=M-1\n";
}

void Translator::decrementStackPointerAndAssignToA() {
  out_stream_ << "@SP\n";
  out_stream_ << "AM=M-1\n";
}

void Translator::decrementStackPointerAndAssignToD() {
  decrementStackPointerAndAssignToA();
  out_stream_ << "D=M\n";
}

void Translator::decrementRegisterAndAssignToSegment(
  std::string register_name, std::string segment_name) {
  out_stream_ << "@" << register_name << "\n";
  out_stream_ << "AM=M-1\n";
  out_stream_ << "D=M\n";
  out_stream_ << "@" << segment_name << "\n";
  out_stream_ << "M=D\n";
}

void Translator::createLabel(std::string label_str) {
  out_stream_ << "(";
  addLabelString(label_str);
  out_stream_ << ")" << "\n";
}

void Translator::atLabelCommand(std::string label_str) {
  out_stream_ << "@";
  addLabelString(label_str);
  out_stream_ << "\n";
}

void Translator::addLabelString(std::string label_str) {
  out_stream_ << curr_function_;
  if (curr_function_.compare("") != 0)
    out_stream_ << "$";
  out_stream_ << label_str;
}

void Translator::addReturnAddress() {
  out_stream_ << curr_function_;
  if (curr_function_.compare("") != 0)
    out_stream_ << "$";
  out_stream_ << "ret." << func_calls_;
}

void Translator::pushValueInRegisterA() {
  out_stream_ << "D=A\n";
  pushValueInRegisterD();
}

void Translator::pushValueInRegisterM() {
  out_stream_ << "D=M\n";
  pushValueInRegisterD();
}

void Translator::pushValueInRegisterD() {
  //SP++; *(SP-1) = D
  stackPointerIncrementInstruction();
  out_stream_ << "A=M-1\n";
  out_stream_ << "M=D\n";
}
