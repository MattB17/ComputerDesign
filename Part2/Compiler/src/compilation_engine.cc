#include "compilation_engine.h"

#include <string>

#include "util.h"

CompilationEngine::CompilationEngine(std::string jack_file)
  : tokenizer_(std::make_unique<Tokenizer>(jack_file))
{
  xml_stream_.open(jackFileToXmlFile(jack_file));
}

void CompilationEngine::compile() {
  while (tokenizer_->hasMoreTokens()) {
    tokenizer_->advance();
    writeTokenWithTag();
    xml_stream_ << '\n';
  }
}

void CompilationEngine::writeTokenWithTag() {
  std::string tag;
  switch (tokenizer_->getTokenType()) {
    case TokenType::KEYWORD:
      tag = "keyword";
      writeOpenTag(tag);
      xml_stream_ << keywordToString(tokenizer_->getKeyword());
      break;
    case TokenType::SYMBOL:
      tag = "symbol";
      writeOpenTag(tag);
      xml_stream_ << tokenizer_->getSymbol();
      break;
    case TokenType::IDENTIFIER:
      tag = "identifier";
      writeOpenTag(tag);
      xml_stream_ << tokenizer_->getIdentifier();
      break;
    case TokenType::INT_CONST:
      tag = "intConstant";
      writeOpenTag(tag);
      xml_stream_ << tokenizer_->getIntVal();
      break;
    case TokenType::STRING_CONST:
      tag = "stringConstant";
      writeOpenTag(tag);
      xml_stream_ << tokenizer_->getStringVal();
      break;
    default:
      tag = "unknown";
      writeOpenTag(tag);
  }
  writeCloseTag(tag);
}

void CompilationEngine::writeOpenTag(std::string tag) {
  xml_stream_ << "<" << tag << "> ";
}

void CompilationEngine::writeCloseTag(std::string tag) {
  xml_stream_ << " </" << tag << ">";
}
