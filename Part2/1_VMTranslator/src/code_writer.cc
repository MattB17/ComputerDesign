#include "code_writer.h"

CodeWriter::CodeWriter(std::string assembly_file) {
  assembly_stream_.open(assembly_file);
}

CodeWriter::writeCommandComment(std::string command) {
  assembly_stream_ << "// " << command << "\n";
}
