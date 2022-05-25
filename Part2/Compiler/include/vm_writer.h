// Used to handle writing compiled jack code to a VM file.
#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <fstream>
#include <string>

#include "segment.h"
#include "symbol.h"

class VMWriter {
public:
  VMWriter(std::string jack_file);
  VMWriter(const VMWriter&) = delete;
  VMWriter &operator=(const VMWriter&) = delete;
  VMWriter(VMWriter&&) = delete;
  VMWriter &operator=(VMWriter&&) = delete;
  ~VMWriter() {}

  void writePush(Segment memory_segment, int idx);

  void writePop(Segment memory_segment, int idx);

  void writeArithmetic(OpCommand op_command);

  void writeLabel(std::string label);

  void writeGoTo(std::string label);

  void writeIfGoTo(std::string label);

  void writeCall(std::string function_name, int n_locals);

  void writeFunction(std::string function_name, int n_args);

  void writeReturn();

  void close();
private:
  std::ofstream vm_stream_;
};

#endif  // VM_WRITER_H
