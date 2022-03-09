#include "translator.h"

#include <sstream>

Translator::Translator() : label_idx_(0) {}

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
  out_stream << "@SP\n";
  out_stream << "M=M+1\n";

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
  out_stream << "@SP\n";
  out_stream << "M=M-1\n";

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
  out_stream << "@SP\n";
  out_stream << "M=M-1\n";

  label_idx_++;
  return out_stream.str();
}
