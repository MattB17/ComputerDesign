// A class to translate VM Commands into assembly commands and write the
// output to a file stream.
#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <string>

class Translator {
public:
  Translator();
  Translator(const Translator&) = delete;
  Translator &operator=(const Translator&) = delete;
  Translator(Translator&&) = delete;
  Translator &operator=(Translator&&) = delete;
  ~Translator() {}

  // translates the VM instruction `push constant i`
  std::string pushConstant(int i);

  // translates the VM binary arithmetic operation given by `operation`. One
  // of `add` or `sub`
  std::string binaryArithmetic(std::string operation);

  // translates the VM `neg` operation
  std::string negate(std::string operation);

  // translates a VM comparison operation. One of `eq`, `lt`, or `gt`
  std::string comparison(std::string compare_type);

private:
  int label_idx_;
};

#endif  // TRANSLATOR_H
