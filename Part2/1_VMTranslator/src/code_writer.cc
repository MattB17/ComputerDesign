#include "code_writer.h"

CodeWriter::CodeWriter(std::string assembly_file)
  : translator_(std::make_unique<Translator>())
{
  assembly_stream_.open(assembly_file);
}

void CodeWriter::writeCommandComment(std::string command) {
  assembly_stream_ << "// " << command << "\n";
}

void CodeWriter::writePushPop(
  Operation command, std::string segment, int val) {
  if (command == Operation::PUSH && segment.compare("constant") == 0) {
    assembly_stream_ << translator_->pushConstant(val);
  }
}

void CodeWriter::writeArithmetic(std::string arithmetic_command) {
  assembly_stream_ << translator_->translateArithmeticOperation(
    arithmetic_command);
}
