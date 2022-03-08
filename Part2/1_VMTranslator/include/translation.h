// A set of methods to translate VM Commands into assembly commands
#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <fstream>

void TranslatePushConstant(std::ofstream& assembly_stream, int i);

void TranslateBinaryArithmetic(
  std::ofstream& assembly_stream, std::string operation);

#endif  // TRANSLATION_H
