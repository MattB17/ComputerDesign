#include "translation.h"

void PushConstant(std::ofstream& assembly_stream, int i) {
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
