// Used to handle the compilation process of a single Jack file. It accepts
// the Jack file as input, creates a tokenizer, and parses the input, writing
// the compiled output to an XML file.
#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include <memory>
#include <string>

#include "tokenizer.h"

class CompilationEngine {
public:
  CompilationEngine(std::string jack_file);
  CompilationEngine(const CompilationEngine&) = delete;
  CompilationEngine &operator=(const CompilationEngine&) = delete;
  CompilationEngine(CompilationEngine&&) = delete;
  CompilationEngine &operator=(CompilationEngine&&) = delete;
  ~CompilationEngine() {}
private:
  std::unique_ptr<Tokenizer> tokenizer_;
  std::ofstream xml_stream_;
};

#endif  // COMPILATION_ENGINE_H
