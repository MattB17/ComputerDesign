#include <iostream>
#include <string>

#include "compilation_engine.h"
#include "tokenizer.h"
#include "token_type.h"

int main(int argc, char** argv) {
  if (argc > 1) {
    std::string jack_file = ((std::string)argv[1]);
    CompilationEngine compilation_engine(jack_file);
    compilation_engine.compileTemp();
  }
  return 0;
}
