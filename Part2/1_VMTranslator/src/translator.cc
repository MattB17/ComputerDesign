#include "translator.h"

#include <sstream>

Translator::Translator(std::string static_segment_)
  : label_idx_(0),  static_segment_(static_segment_) {}

std::string Translator::translateArithmeticOperation(std::string operation) {
  std::stringstream out_stream;
  if (operation.compare("add") == 0) {
    // D = x + y
    return translateCombination(out_stream, "D=D+M");
  } else if (operation.compare("sub") == 0) {
    // D = x - y
    return translateCombination(out_stream, "D=M-D");
  } else if (operation.compare("and") == 0) {
    // D = x & y
    return translateCombination(out_stream, "D=D&M");
  } else if (operation.compare("or") == 0) {
    // D = x | y
    return translateCombination(out_stream, "D=D|M");
  } else if (operation.compare("neg") == 0) {
    // M = -x
    return translateNegation(out_stream, "M=-M");
  } else if (operation.compare("not") == 0) {
    // M = !x
    return translateNegation(out_stream, "M=!M");
  } else if (operation.compare("eq") == 0) {
    // D = x - y, jump if D == 0
    return translateComparison(out_stream, "D;JEQ");
  } else if (operation.compare("lt") == 0) {
    // D = x - y, jump if D < 0
    return translateComparison(out_stream, "D;JLT");
  } else if (operation.compare("gt") == 0) {
    // D = x - y, jump if D > 0
    return translateComparison(out_stream, "D;JGT");
  }
  return "";
}

std::string Translator::translatePushOperation(std::string segment, int i) {
  std::stringstream out_stream;
  if (segment.compare("constant") == 0) {
    return pushConstant(out_stream, i);
  } else if (segment.compare("local") == 0) {
    out_stream << "@LCL\n";
    return pushSegment(out_stream, i);
  } else if (segment.compare("argument") == 0) {
    out_stream << "@ARG\n";
    return pushSegment(out_stream, i);
  } else if (segment.compare("this") == 0) {
    out_stream << "@THIS\n";
    return pushSegment(out_stream, i);
  } else if (segment.compare("that") == 0) {
    out_stream << "@THAT\n";
    return pushSegment(out_stream, i);
  } else if (segment.compare("temp") == 0) {
    out_stream << "@5\n";
    return pushTemp(out_stream, i);
  } else if (segment.compare("static") == 0) {
    out_stream << "@" << static_segment_ << "." << i << "\n";
    return pushMemoryContentsToStack(out_stream);
  } else if (segment.compare("pointer") == 0) {
    if (i == 0) {
      out_stream << "@THIS\n";
    } else {
      out_stream << "@THAT\n";
    }
    return pushMemoryContentsToStack(out_stream);
  }
  return "";
}

std::string Translator::translatePopOperation(std::string segment, int i) {
  std::stringstream out_stream;
  if (segment.compare("local") == 0) {
    out_stream << "@LCL\n";
    return popSegment(out_stream, i);
  } else if (segment.compare("argument") == 0) {
    out_stream << "@ARG\n";
    return popSegment(out_stream, i);
  } else if (segment.compare("this") == 0) {
    out_stream << "@THIS\n";
    return popSegment(out_stream, i);
  } else if (segment.compare("that") == 0) {
    out_stream << "@THAT\n";
    return popSegment(out_stream, i);
  } else if (segment.compare("temp") == 0) {
    out_stream << "@5\n";
    return popTemp(out_stream, i);
  } else if (segment.compare("static") == 0) {
    return popStatic(out_stream, i);
  } else if (segment.compare("pointer") == 0) {
    return popPointer(out_stream, i);
  }
  return "";
}

std::string Translator::translateCombination(
  std::stringstream& out_stream, std::string combination_expression) {
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

std::string Translator::translateNegation(
  std::stringstream& out_stream, std::string negation_expression) {
  // *(SP-1) = -(*(SP-1)) - that is just directly access the variable
  // and negate, no need to pop it off and push it back
  out_stream << "@SP\n";
  out_stream << "A=M-1\n";
  // M stores the variable x so write out the negation expression
  out_stream << negation_expression << "\n";

  return out_stream.str();
}

std::string Translator::translateComparison(
  std::stringstream& out_stream, std::string comparison_expression) {
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

std::string Translator::pushConstant(std::stringstream& out_stream, int i) {
  // D = i
  out_stream << "@" << i << "\n";
  out_stream << "D=A\n";

  // SP++; *(SP-1) = D
  out_stream << stackPointerIncrementInstruction();
  out_stream << "A=M-1\n";
  out_stream << "M=D\n";

  return out_stream.str();
}

std::string Translator::pushSegment(std::stringstream& out_stream, int i) {
  // D = RAM[@segment] (A already points to @segment)
  out_stream << "D=M\n";

  return addOffsetAndPushToStack(out_stream, /*offset=*/i);
}

std::string Translator::pushTemp(std::stringstream& out_stream, int i) {
  // D = 5 (A = @temp, that is A = 5)
  out_stream << "D=A\n";

  return addOffsetAndPushToStack(out_stream, /*offset=*/i);
}

std::string Translator::pushMemoryContentsToStack(
  std::stringstream& out_stream) {
  // Put the current memory contents into D
  out_stream << "D=M\n";

  // SP++; *(SP-1) = D
  out_stream << stackPointerIncrementInstruction();
  out_stream << "A=M-1\n";
  out_stream << "M=D\n";

  return out_stream.str();
}

std::string Translator::addOffsetAndPushToStack(
  std::stringstream& out_stream, int offset) {
  // A = D + offset (where D = RAM[@segment] so A = RAM[@segment] + offset).
  out_stream << "@" << offset << "\n";
  out_stream << "A=D+A\n";

  // D = RAM[A] (where A = @segment + i)
  out_stream << "D=M\n";

  // SP++; *(SP-1) = D
  out_stream << stackPointerIncrementInstruction();
  out_stream << "A=M-1\n";
  out_stream << "M=D\n";

  return out_stream.str();
}

std::string Translator::popSegment(std::stringstream& out_stream, int i) {
  // D = RAM[@segment] (A already has the value @segment)
  out_stream << "D=M\n";

  return addOffsetAndPopFromStack(out_stream, /*offset=*/i);
}

std::string Translator::popTemp(std::stringstream& out_stream, int i) {
  // D = 5 (A already stores 5, the start of the temp segment)
  out_stream << "D=A\n";

  return addOffsetAndPopFromStack(out_stream, /*offset=*/i);
}

std::string Translator::popStatic(std::stringstream& out_stream, int i) {
  out_stream << decrementStackPointerAndAssignToD();

  // @Foo.i = D where `Foo` is the name of the static segment
  out_stream << "@" << static_segment_ << "." << i << "\n";
  out_stream << "M=D\n";

  return out_stream.str();
}

std::string Translator::popPointer(std::stringstream& out_stream, int i) {
  out_stream << decrementStackPointerAndAssignToD();

  // @Ptr = D where `Ptr` is either `THIS` or `THAT` depending on `i`.
  if (i == 0) {
    out_stream << "@THIS\n";
  } else {
    out_stream << "@THAT\n";
  }
  out_stream << "M=D\n";

  return out_stream.str();
}

std::string Translator::addOffsetAndPopFromStack(
  std::stringstream& out_stream, int offset) {
  // D = D + i (where D = RAM[@segment], so D = RAM[@segment] + i)
  out_stream << "@" << offset << "\n";
  out_stream << "D=D+A\n";

  // SP--; AM = *SP
  out_stream << decrementStackPointerAndAssignToA();

  // add this element to D so now D stores `@segment + i + val` where
  // `val` is the value popped off the stack
  out_stream << "D=D+M\n";

  // update A to `@segment + i`
  out_stream << "A=D-M\n";

  // update RAM[@segment + i] to `val` which is D - A
  out_stream << "M=D-A\n";

  return out_stream.str();
}

std::string Translator::stackPointerIncrementInstruction() {
  return "@SP\nM=M+1\n";
}

std::string Translator::stackPointerDecrementInstruction() {
  return "@SP\nM=M-1\n";
}

std::string Translator::decrementStackPointerAndAssignToA() {
  return "@SP\nAM=M-1\n";
}

std::string Translator::decrementStackPointerAndAssignToD() {
  return "@SP\nAM=M-1\nD=M\n";
}
