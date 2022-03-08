#include "translation.h"

void TranslatePushConstant(std::ofstream& assembly_stream, int i) {
  // D = i
  assembly_stream << "@" << i << "\n";
  assembly_stream << "D=A\n";

  // *SP = D
  assembly_stream << "@SP\n";
  assembly_stream << "A=M\n";
  assembly_stream << "M=D\n";

  // SP++
  assembly_stream << "@SP\n";
  assembly_stream << "M=M+1\n";
}

void TranslateBinaryArithmetic(
  std::ofstream& assembly_stream, std::string operation) {
  // D = *(SP-1) - this is the variable y
  assembly_stream << "@SP\n";
  assembly_stream << "A=M-1\n";
  assembly_stream << "D=M\n";

  // A = *(SP-2) - this is now the address of x
  assembly_stream << "A=A-1\n";
  if (operation.compare("add") == 0) {
    // D = y + x (D already stores the value of y)
    assembly_stream << "D=D+M\n";
  } else {
    // D = x - y (D already stores the value of y)
    assembly_stream << "D=M-D\n";
  }

  // *(SP-2) = D
  assembly_stream << "M=D\n";

  // SP--;
  assembly_stream << "@SP\n";
  assembly_stream << "M=M-1\n";
}

void TranslateNeg(std::ofstream& assembly_stream) {
  // *(SP-1) = -(*(SP-1)) - that is just directly access the variable
  // and negate, no need to pop it off and push it back
  assembly_stream << "@SP\n";
  assembly_stream << "A=M-1\n";
  assembly_stream << "M=-M\n";
}

void TranslateEq(std::ofstream& assembly_stream, int label_idx) {
  // D = *(SP-1) - this is the variable y
  assembly_stream << "@SP\n";
  assembly_stream << "A=M-1\n";
  assembly_stream << "D=M\n";

  // A = *(SP-2) - this is now the address of x
  assembly_stream << "A=A-1\n";
  // D = x - y (D already stores the value of y)
  assembly_stream << "D=M-D\n";

  // put value of true in *(SP-2)
  assembly_stream << "M=-1\n";

  // if D = 0 (x == y) we already put true in the stack so jump to
  // the cleanup
  assembly_stream << "@CLEANUP" << label_idx << "\n";
  assembly_stream << "D;JEQ\n";

  // otherwise, flip true to false in the stack position
  assembly_stream << "@SP\n";
  assembly_stream << "A=M-1\n";
  assembly_stream << "A=A-1\n";
  assembly_stream << "M=M+1\n";

  // SP--;
  assembly_stream << "(CLEANUP" << label_idx << ")\n";
  assembly_stream << "@SP\n";
  assembly_stream << "M=M-1\n";
}
