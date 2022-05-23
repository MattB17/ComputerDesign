#include "vm_writer.h"

#include "util.h"

VMWriter::VMWriter(std::string jack_file) {
  vm_stream_.open(jackFileToOutputFile(jack_file, ".vm"));
}

void VMWriter::close() {
  vm_stream_.close();
}
