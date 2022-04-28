#include <iostream>
#include <string>

#include "tokenizer.h"
#include "token_type.h"

int main(int argc, char** argv) {
  if (argc > 1) {
    std::string jack_file = ((std::string)argv[1]);
    Tokenizer tokenizer(jack_file);
    TokenType curr_token_type;
    while (tokenizer.hasMoreTokens()) {
      tokenizer.advance();
      curr_token_type = tokenizer.getTokenType();
      switch (curr_token_type) {
        case TokenType::KEYWORD:
          std::cout << "Keyword: " << tokenizer.getKeyword();
        case TokenType::SYMBOL:
          std::cout << "Symbol: " << tokenizer.getSymbol();
        case TokenType::IDENTIFIER:
          std::cout << "Identifier: " << tokenizer.getIdentifier();
        case TokenType::INT_CONST:
          std::cout << "Int: " << tokenizer.getIntVal();
        case TokenType::STRING_CONST:
          std::cout << "String: " << tokenizer.getStringVal();
        default:
          std::cout << "Unknown" << std::endl;
      }
    }
  }
  return 0;
}
