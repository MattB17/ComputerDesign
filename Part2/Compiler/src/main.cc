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
          std::cout << "Keyword: " << tokenizer.getKeyword() << std::endl;
          break;
        case TokenType::SYMBOL:
          std::cout << "Symbol: " << tokenizer.getSymbol() << std::endl;
          break;
        case TokenType::IDENTIFIER:
          std::cout << "Identifier: " << tokenizer.getIdentifier() << std::endl;
          break;
        case TokenType::INT_CONST:
          std::cout << "Int: " << tokenizer.getIntVal() << std::endl;
          break;
        case TokenType::STRING_CONST:
          std::cout << "String: " << tokenizer.getStringVal() << std::endl;
          break;
        default:
          std::cout << "Unknown" << std::endl;
      }
    }
  }
  return 0;
}
