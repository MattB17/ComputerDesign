#include "code_writer.h"

#include "translation.h"

CodeWriter::CodeWriter(std::string assembly_file) : label_count_(0) {
  assembly_stream_.open(assembly_file);
}

void CodeWriter::writeCommandComment(std::string command) {
  assembly_stream_ << "// " << command << "\n";
}

void CodeWriter::writePushPop(
  Operation command, std::string segment, int val) {
  if (command == Operation::PUSH && segment.compare("constant") == 0) {
    TranslatePushConstant(assembly_stream_, val);
  }
}

void CodeWriter::writeArithmetic(std::string arithmetic_command) {
  if (arithmetic_command.compare("add") == 0 ||
      arithmetic_command.compare("sub") == 0) {
    TranslateBinaryArithmetic(assembly_stream_, arithmetic_command);
  } else if (arithmetic_command.compare("neg") == 0) {
    TranslateNeg(assembly_stream_);
  } else if (arithmetic_command.compare("eq") == 0) {
    TranslateEq(assembly_stream_, label_count_);
    label_count_++;
  }
}
