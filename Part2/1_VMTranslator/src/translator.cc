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

std::string Translator::binaryArithmetic(std::string operation) {
  std::stringstream out_stream;
  // D = *(SP-1) - this is the variable y
  out_stream << "@SP\n";
  out_stream << "A=M-1\n";
  out_stream << "D=M\n";

  // A = *(SP-2) - this is now the address of x
  out_stream << "A=A-1\n";
  // M stores the value of X and D stores the value of y
  if (operation.compare("add") == 0) {
    // D = y + x
    out_stream << "D=D+M\n";
  } else if (operation.compare("sub") == 0 ) {
    // D = x - y
    out_stream << "D=M-D\n";
  } else if (operation.compare("and") == 0) {
    // D = y & x
    out_stream << "D=D&M\n";
  } else {
    // D = y | x
    out_stream << "D=D|M\n";
  }

  // *(SP-2) = D
  out_stream << "M=D\n";

  // SP--;
  out_stream << "@SP\n";
  out_stream << "M=M-1\n";

  return out_stream.str();
}

std::string Translator::negate(std::string operation) {
  std::stringstream out_stream;
  // *(SP-1) = -(*(SP-1)) - that is just directly access the variable
  // and negate, no need to pop it off and push it back
  out_stream << "@SP\n";
  out_stream << "A=M-1\n";
  if (operation.compare("neg") == 0) {
    out_stream << "M=-M\n";
  } else {
    out_stream << "M=!M\n";
  }

  return out_stream.str();
}

std::string Translator::comparison(std::string compare_type) {
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

  // if D = 0 (x == y) we already put true in the stack so jump to
  // the cleanup
  out_stream << "@CLEANUP" << label_idx_ << "\n";
  // depending on the comparison, check for equality, less than, or
  // greater than.
  if (compare_type.compare("eq") == 0) {
    out_stream << "D;JEQ\n";
  } else if (compare_type.compare("lt") == 0) {
    out_stream << "D;JLT\n";
  } else {
    out_stream << "D;JGT\n";
  }

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
