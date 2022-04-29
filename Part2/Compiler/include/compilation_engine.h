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

  // Runs the compilation process. Translating the jack file to an XML file.
  void compile();
private:
  // Writes the current token to the XML stream with the appropriate tag.
  void writeTokenWithTag();

  // Writes the XML opening tag for `tag`.
  void writeOpenTag(std::string tag);

  // Writes the XML closing tag for `tag`.
  void writeCloseTag(std::string tag);

  // The tokenizer used to retrieve tokens from the jack file being compiled.
  std::unique_ptr<Tokenizer> tokenizer_;

  // A stream representing the to which the compilation engine writes output.
  std::ofstream xml_stream_;
};

#endif  // COMPILATION_ENGINE_H
