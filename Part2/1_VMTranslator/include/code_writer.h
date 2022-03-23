// Generates assembly code from parsed vm commands and writes the output
// to an assembly file.
#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#include <string>
#include <fstream>
#include <memory>

#include "operation.h"
#include "translator.h"

class CodeWriter {
public:
  CodeWriter(std::string assembly_file);
  CodeWriter(const CodeWriter&) = delete;
  CodeWriter &operator=(const CodeWriter&) = delete;
  CodeWriter(CodeWriter&&) = delete;
  CodeWriter &operator=(CodeWriter&&) = delete;
  ~CodeWriter() {}

  void setFileName(std::string file_name);

  void writeCommandComment(std::string command);

  void writeInit();

  void writeArithmetic(std::string arithmetic_command);

  void writePushPop(Operation command, std::string segment, int val);

  void writeLabel(std::string label_str);

  void writeGoTo(std::string label_str);

  void writeIf(std::string label_str);

  void writeFunction(std::string function_name, int n_vars);

  void writeReturn();

  void writeCall(std::string function_name, int n_args);

  void close() { assembly_stream_.close(); }

protected:
  std::ofstream assembly_stream_;
  std::unique_ptr<Translator> translator_;
};

#endif  // CODE_WRITER_H
