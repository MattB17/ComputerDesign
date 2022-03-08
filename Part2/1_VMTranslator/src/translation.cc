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
    assembly_stream << "D=M-D\n";
  }

  // *(SP-1) = D
  assembly_stream << "M=D\n";

  // SP--;
  assembly_stream << "@SP\n";
  assembly_stream << "M=M-1";
}
