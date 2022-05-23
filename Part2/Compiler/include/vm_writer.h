// Used to handle writing compiled jack code to a VM file.
#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <fstream>
#include <string>

class VMWriter {
public:
  VMWriter(std::string jack_file);
  VMWriter(const VMWriter&) = delete;
  VMWriter &operator=(const VMWriter&) = delete;
  VMWriter(VMWriter&&) = delete;
  VMWriter &operator=(VMWriter&&) = delete;
  ~VMWriter() {}

  void close();
private:
  std::ofstream vm_stream_;
};

#endif  // VM_WRITER_H
