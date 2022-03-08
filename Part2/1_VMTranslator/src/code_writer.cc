#include "code_writer.h"

#include "translation.h"

CodeWriter::CodeWriter(std::string assembly_file) {
  assembly_stream_.open(assembly_file);
}

void CodeWriter::writeCommandComment(std::string command) {
  assembly_stream_ << "// " << command << "\n";
}

void CodeWriter::writePushPop(
  Operation command, std::string segment, int val) {
  if (command == Operation::PUSH && segment.compare("constant") == 0) {
    PushConstant(assembly_stream_, val);
  }
}

void CodeWriter::writeArithmetic(std::string arithmetic_command) {}
