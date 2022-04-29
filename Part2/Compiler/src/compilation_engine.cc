#include "compilation_engine.h"

#include "util.h"

CompilationEngine::CompilationEngine(std::string jack_file)
  : tokenizer_(std::make_unique<Tokenizer>(jack_file))
{
  xml_stream_.open(jackFileToXmlFile(jack_file));
}
