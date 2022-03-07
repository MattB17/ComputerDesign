// Generates assembly code from parsed vm commands and writes the output
// to an assembly file.
#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include <string>
#include <fstream>

#include "operation.h"

class CodeWriter {
public:
  CodeWriter(std::string assembly_file);
  CodeWriter(const CodeWriter&) = delete;
  CodeWriter &operator=(const CodeWriter&) = delete;
  CodeWriter(CodeWriter&&) = delete;
  CodeWriter &operator=(CodeWriter&&) = delete;
  ~CodeWriter() {}

  void writeCommandComment(std::string command);

  void writeArithmetic(std::string arithmetic_command);

  void writePushPop(Operation command, std::string segment, int idx);

  void close() { assembly_stream_.close(); }

private:
  std::ostream assembly_stream_;
};

#endif  // CODE_WRITER_H
