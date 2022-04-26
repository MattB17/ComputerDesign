#include "tokenizer.h"

Tokenizer::Tokenizer(std::string jack_file) {
  jack_stream_.open(jack_file);
}
