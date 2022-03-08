// A set of methods to translate VM Commands into assembly commands
#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <fstream>

void PushConstant(std::ofstream& assembly_stream, int i);

#endif  // TRANSLATION_H
