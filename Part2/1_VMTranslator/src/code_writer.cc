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
    assembly_stream_ << translator_->translatePushOperation(segment, val);
  } else {
    assembly_stream_ << translator_->translatePopOperation(segment, val);
  }
}

void CodeWriter::writeArithmetic(std::string arithmetic_command) {
  assembly_stream_ << translator_->translateArithmeticOperation(
    arithmetic_command);
}

void CodeWriter::writeLabel(std::string label_str) {
  assembly_stream_ << translator_->translateLabelOperation(label_str);
}

void CodeWriter::writeGoTo(std::string label_str) {
  assembly_stream_ << translator_->translateGoToOperation(label_str);
}

void CodeWriter::writeIf(std::string label_str) {
  assembly_stream_ << translator_->translateIfGoToOperation(label_str);
}

void CodeWriter::writeFunction(std::string function_name, int n_vars) {
  assembly_stream_ << translator_->translateFunctionOperation(
    function_name, n_vars);
}

void CodeWriter::writeReturn() {
  assembly_stream_ << translator_->translateReturnOperation();
}
