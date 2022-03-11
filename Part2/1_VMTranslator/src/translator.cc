#include "translator.h"

#include <sstream>

Translator::Translator(std::string static_segment_)
  : label_idx_(0),  static_segment_(static_segment_) {}

std::string Translator::pushConstant(int i) {
  std::stringstream out_stream;
  // D = i
  out_stream << "@" << i << "\n";
  out_stream << "D=A\n";

  // *SP = D
  out_stream << "@SP\n";
  out_stream << "A=M\n";
  out_stream << "M=D\n";

  // SP++
  out_stream << stackPointerIncrementInstruction();

  return out_stream.str();
}

std::string Translator::pushSegment(std::string segment, int i) {
  std::stringstream out_stream;
  // D = @segment
  if (segment.compare("local") == 0) {
    out_stream << "@LCL\n";
  } else if (segment.compare("argument") == 0) {
    out_stream << "@ARG\n";
  } else if (segment.compare("this") == 0) {
    out_stream << "@THIS\n";
  } else {
    out_stream << "@THAT\n";
  }
  out_stream << "D=M\n";

  // A = D + i (where D = *segment, so A = *segment + i)
  out_stream << "@" << i << "\n";
  out_stream << "A=D+A\n";

  // D = RAM[A] (where A = @segment + i)
  out_stream << "D=M\n";

  // *SP = D
  out_stream << "@SP\n";
  out_stream << "A=M\n";
  out_stream << "M=D\n";

  // SP++
  out_stream << stackPointerIncrementInstruction();

  return out_stream.str();
}

std::string Translator::pushTemp(int i) {
  std::stringstream out_stream;
  // D = 5
  out_stream << "@5\n";
  out_stream << "D=A\n";

  // A = D + i (where D = 5 so A = 5 + i)
  out_stream << "@" << i << "\n";
  out_stream << "A=D+A\n";

  // D = RAM[A] (where A = @segment + i)
  out_stream << "D=M\n";

  // *SP = D
  out_stream << "@SP\n";
  out_stream << "A=M\n";
  out_stream << "M=D\n";

  // SP++
  out_stream << stackPointerIncrementInstruction();

  return out_stream.str();
}

std::string Translator::pushStatic(int i) {
  std::stringstream out_stream;
  // D = @Foo.i where `Foo` is the name of the static segment
  out_stream << "@" << static_segment_ << "." << i << "\n";
  out_stream << "D=M\n";

  // *SP = D
  out_stream << "@SP\n";
  out_stream << "A=M\n";
  out_stream << "M=D\n";

  // SP++
  out_stream << stackPointerIncrementInstruction();

  return out_stream.str();
}

std::string Translator::popSegment(std::string segment, int i) {
  std::stringstream out_stream;
  // D = @segment
  if (segment.compare("local") == 0) {
    out_stream << "@LCL\n";
  } else if (segment.compare("argument") == 0) {
    out_stream << "@ARG\n";
  } else if (segment.compare("this") == 0) {
    out_stream << "@THIS\n";
  } else {
    out_stream << "@THAT\n";
  }
  out_stream << "D=M\n";

  // D = D + i (where D = @segment, so D = @segment + i)
  out_stream << "@" << i << "\n";
  out_stream << "D=D+A\n";

  // SP--; M = *SP
  out_stream << stackPointerDecrementInstruction();

  // go to the memory location of the last element
  out_stream << "A=M\n";

  // add this element to D so now D stores `@segment + i + val` where
  // `val` is the value popped off the stack
  out_stream << "D=D+M\n";

  // update A to `@segment + i`
  out_stream << "A=D-M\n";

  // update RAM[@segment + i] to `val` which is D - A
  out_stream << "M=D-A\n";

  return out_stream.str();
}

std::string Translator::popTemp(int i) {
  std::stringstream out_stream;
  // D = 5
  out_stream << "@5\n";
  out_stream << "D=A\n";

  // D = D + i (where D = 5 so D = 5 + i)
  out_stream << "@" << i << "\n";
  out_stream << "D=D+A\n";

  // SP--; M = *SP
  out_stream << stackPointerDecrementInstruction();

  // go to the memory location of the last element
  out_stream << "A=M\n";

  // add this element to D so now D stores `5 + i + val` where `val` is the
  // value popped off the stack
  out_stream << "D=D+M\n";

  // update A to `5 + i`
  out_stream << "A=D-M\n";

  // update RAM[5 + i] to `val` which is D - A
  out_stream << "M=D-A\n";

  return out_stream.str();
}

std::string Translator::popStatic(int i) {
  std::stringstream out_stream;
  // D = *SP; SP--;
  out_stream << stackPointerDecrementInstruction();
  out_stream << "A=M\n";
  out_stream << "D=M\n";

  // @Foo.i = D where `Foo` is the name of the static segment
  out_stream << "@" << static_segment_ << "." << i << "\n";
  out_stream << "M=D\n";

  return out_stream.str();
}

std::string Translator::translateArithmeticOperation(std::string operation) {
  if (operation.compare("add") == 0) {
    // D = x + y
    return translateCombination("D=D+M");
  } else if (operation.compare("sub") == 0) {
    // D = x - y
    return translateCombination("D=M-D");
  } else if (operation.compare("and") == 0) {
    // D = x & y
    return translateCombination("D=D&M");
  } else if (operation.compare("or") == 0) {
    // D = x | y
    return translateCombination("D=D|M");
  } else if (operation.compare("neg") == 0) {
    // M = -x
    return translateNegation("M=-M");
  } else if (operation.compare("not") == 0) {
    // M = !x
    return translateNegation("M=!M");
  } else if (operation.compare("eq") == 0) {
    // D = x - y, jump if D == 0
    return translateComparison("D;JEQ");
  } else if (operation.compare("lt") == 0) {
    // D = x - y, jump if D < 0
    return translateComparison("D;JLT");
  } else if (operation.compare("gt") == 0) {
    // D = x - y, jump if D > 0
    return translateComparison("D;JGT");
  }
  return "";
}

std::string Translator::translateCombination(
  std::string combination_expression) {
  std::stringstream out_stream;
  // D = *(SP-1) - this is the variable y
  out_stream << "@SP\n";
  out_stream << "A=M-1\n";
  out_stream << "D=M\n";

  // A = *(SP-2) - this is now the address of x
  out_stream << "A=A-1\n";
  // M stores the value of X and D stores the value of y.
  // So add the line with the combination expression
  out_stream << combination_expression << "\n";

  // *(SP-2) = D
  out_stream << "M=D\n";

  // SP--;
  out_stream << stackPointerDecrementInstruction();

  return out_stream.str();
}

std::string Translator::translateNegation(std::string negation_expression) {
  std::stringstream out_stream;
  // *(SP-1) = -(*(SP-1)) - that is just directly access the variable
  // and negate, no need to pop it off and push it back
  out_stream << "@SP\n";
  out_stream << "A=M-1\n";
  // M stores the variable x so write out the negation expression
  out_stream << negation_expression << "\n";

  return out_stream.str();
}

std::string Translator::translateComparison(
  std::string comparison_expression) {
  std::stringstream out_stream;
  // D = *(SP-1) - this is the variable y
  out_stream << "@SP\n";
  out_stream << "A=M-1\n";
  out_stream << "D=M\n";

  // A = *(SP-2) - this is now the address of x
  out_stream << "A=A-1\n";
  // D = x - y (D already stores the value of y)
  out_stream << "D=M-D\n";

  // put value of true in *(SP-2)
  out_stream << "M=-1\n";

  // if D = x - y and we already put true in the stack position.
  // So if the comparison evaluates to true jump to cleanup
  out_stream << "@CLEANUP" << label_idx_ << "\n";
  out_stream << comparison_expression << "\n";

  // otherwise, flip true to false in the stack position
  out_stream << "@SP\n";
  out_stream << "A=M-1\n";
  out_stream << "A=A-1\n";
  out_stream << "M=M+1\n";

  // SP--;
  out_stream << "(CLEANUP" << label_idx_ << ")\n";
  out_stream << stackPointerDecrementInstruction();

  label_idx_++;
  return out_stream.str();
}

std::string Translator::stackPointerIncrementInstruction() {
  return "@SP\nM=M+1\n";
}

std::string Translator::stackPointerDecrementInstruction() {
  return "@SP\nM=M-1\n";
}
