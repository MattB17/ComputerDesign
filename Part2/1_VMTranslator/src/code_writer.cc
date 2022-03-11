#include "code_writer.h"

CodeWriter::CodeWriter(std::string assembly_file, std::string file_name)
  : translator_(std::make_unique<Translator>(file_name))
{
  assembly_stream_.open(assembly_file);
}

void CodeWriter::writeCommandComment(std::string command) {
  assembly_stream_ << "// " << command << "\n";
}

void CodeWriter::writePushPop(
  Operation command, std::string segment, int val) {
  if (command == Operation::PUSH) {
    if (segment.compare("constant") == 0) {
      assembly_stream_ << translator_->pushConstant(val);
    } else if (segment.compare("temp") == 0) {
      assembly_stream_ << translator_->pushTemp(val);
    } else if (segment.compare("local") == 0 ||
               segment.compare("argument") == 0 ||
               segment.compare("this") == 0 ||
               segment.compare("that") == 0) {
      assembly_stream_ << translator_->pushSegment(segment, val);
    } else if (segment.compare("static") == 0) {
      assembly_stream_ << translator_->pushStatic(val);
    }
  } else {
    if (segment.compare("temp") == 0) {
      assembly_stream_ << translator_->popTemp(val);
    } else if (segment.compare("local") == 0 ||
               segment.compare("argument") == 0 ||
               segment.compare("this") == 0 ||
               segment.compare("that") == 0) {
      assembly_stream_ << translator_->popSegment(segment, val);
    } else if (segment.compare("static") == 0) {
      assembly_stream_ << translator_->popStatic(val);
    }
  }
}

void CodeWriter::writeArithmetic(std::string arithmetic_command) {
  assembly_stream_ << translator_->translateArithmeticOperation(
    arithmetic_command);
}
