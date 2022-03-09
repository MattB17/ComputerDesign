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
  if (arithmetic_command.compare("add") == 0 ||
      arithmetic_command.compare("sub") == 0 ||
      arithmetic_command.compare("and") == 0 ||
      arithmetic_command.compare("or") == 0) {
    assembly_stream_ << translator_->binaryArithmetic(arithmetic_command);
  } else if (arithmetic_command.compare("neg") == 0 ||
             arithmetic_command.compare("not") == 0) {
    assembly_stream_ << translator_->negate(arithmetic_command);
  } else if (arithmetic_command.compare("eq") == 0 ||
             arithmetic_command.compare("gt") == 0 ||
             arithmetic_command.compare("lt") == 0) {
    assembly_stream_ << translator_->comparison(arithmetic_command);
  }
}
