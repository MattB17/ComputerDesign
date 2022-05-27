#include "vm_writer.h"

#include "util.h"

VMWriter::VMWriter(std::string jack_file) {
  vm_stream_.open(jackFileToOutputFile(jack_file, ".vm"));
}

void VMWriter::writePush(Segment memory_segment, int idx) {
  vm_stream_ << "push " << SegmentToString(memory_segment);
  vm_stream_ << " " << idx << '\n';
}

void VMWriter::writePop(Segment memory_segment, int idx) {
  vm_stream_ << "pop " << SegmentToString(memory_segment);
  vm_stream_ << " " << idx << '\n';
}

void VMWriter::writeArithmetic(OpCommand op_command) {
  vm_stream_ << OpCommandToString(op_command) << '\n';
}

void VMWriter::writeLabel(std::string label) {
  vm_stream_ << "label " << label << '\n';
}

void VMWriter::writeGoTo(std::string label) {
  vm_stream_ << "goto " << label << '\n';
}

void VMWriter::writeIfGoTo(std::string label) {
  vm_stream_ << "if-goto " << label << '\n';
}

void VMWriter::writeCall(std::string function_name, int n_args) {
  vm_stream_ << "call " << function_name << " " << n_args << '\n';
}

void VMWriter::writeFunction(std::string function_name, int n_locals) {
  vm_stream_ << "function " << function_name << " " << n_locals << '\n';
}

void VMWriter::writeReturn() {
  vm_stream_ << "return\n";
}

void VMWriter::close() {
  vm_stream_.close();
}
