#include <filesystem>
#include <string>
#include <vector>

#include "compilation_engine.h"
#include "tokenizer.h"
#include "token_type.h"
#include "util.h"

namespace fs = std::filesystem;

int main(int argc, char** argv) {
  if (argc > 1) {
    std::string file_path = ((std::string)argv[1]);

    std::vector<std::string> jack_files;
    std::string ext = ".jack";
    if (endsInExtension(file_path, ext)) {
      // is a single file.
      jack_files.push_back(file_path);
    } else {
      // is a directory.
      for (auto const &p : fs::recursive_directory_iterator(file_path)) {
        auto jack_file = p.path();
        if (jack_file.extension().compare(ext) == 0) {
          jack_files.push_back(jack_file.string());
        }
      }
    }

    CompilationEngine compilation_engine;
    for (size_t i = 0; i < jack_files.size(); i++) {
      compilation_engine.compile(jack_files[i]);
    }
  }
  return 0;
}
