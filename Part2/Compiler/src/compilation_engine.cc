#include "compilation_engine.h"

#include <string>

#include "exceptions.h"
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

void CompilationEngine::compileVarDec() {
  tokenizer_->nextToken();
  const std::string var_tag = "varDec";
  writeOpenTag(var_tag);
  xml_stream_ << '\n';
  expectKeyword(Keyword::VAR);
  writeTokenWithTag();
  xml_stream_ << '\n';
  tokenizer_->nextToken();
  expectType();
  writeTokenWithTag();
  xml_stream_ << '\n';
  tokenizer_->nextToken();
  expectIdentifier();
  writeTokenWithTag();
  xml_stream_ << '\n';
  tokenizer_->nextToken();
  while (!isStatementEnd()) {
    if ((tokenizer_->getTokenType() == TokenType::SYMBOL) &&
        (tokenizer_->getSymbol() == ',')) {
      writeTokenWithTag();
      xml_stream_ << '\n';
      tokenizer_->nextToken();
      expectIdentifier();
      writeTokenWithTag();
      xml_stream_ << '\n';
      tokenizer_->nextToken();
    } else {
      throw ExpectedStatementEnd(tokenizer_->tokenToString());
    }
  }
  writeTokenWithTag();
  xml_stream_ << '\n';
  writeCloseTag(var_tag);
  xml_stream_ << '\n';
}

void CompilationEngine::writeTokenWithTag() {
  std::string tag;
  switch (tokenizer_->getTokenType()) {
    case TokenType::KEYWORD:
      tag = "keyword";
      writeOpenTag(tag);
      xml_stream_ << " " << keywordToString(tokenizer_->getKeyword());
      break;
    case TokenType::SYMBOL:
      tag = "symbol";
      writeOpenTag(tag);
      xml_stream_ << " " << tokenizer_->getSymbol();
      break;
    case TokenType::IDENTIFIER:
      tag = "identifier";
      writeOpenTag(tag);
      xml_stream_ << " " << tokenizer_->getIdentifier();
      break;
    case TokenType::INT_CONST:
      tag = "intConstant";
      writeOpenTag(tag);
      xml_stream_ << " " << tokenizer_->getIntVal();
      break;
    case TokenType::STRING_CONST:
      tag = "stringConstant";
      writeOpenTag(tag);
      xml_stream_ << " " << tokenizer_->getStringVal();
      break;
    default:
      tag = "unknown";
      writeOpenTag(tag);
  }
  xml_stream_ << " ";
  writeCloseTag(tag);
}

void CompilationEngine::writeOpenTag(std::string tag) {
  xml_stream_ << "<" << tag << ">";
}

void CompilationEngine::writeCloseTag(std::string tag) {
  xml_stream_ << "</" << tag << ">";
}

void CompilationEngine::expectKeyword(Keyword k) {
  if ((tokenizer_->getTokenType() != TokenType::KEYWORD) ||
      (tokenizer_->getKeyword() != k)) {
    throw KeywordNotFound(k, tokenizer_->tokenToString());
  }
  return;
}

void CompilationEngine::expectType() {
  if (tokenizer_->getTokenType() == TokenType::IDENTIFIER) {
    return;
  }
  if ((tokenizer_->getTokenType() == TokenType::KEYWORD) &&
      (IsPrimitiveType(tokenizer_->getKeyword()))) {
    return;
  }
  throw InvalidType(tokenizer_->tokenToString());
}

void CompilationEngine::expectIdentifier() {
  if (tokenizer_->getTokenType() == TokenType::IDENTIFIER) {
    return;
  }
  throw MissingIdentifier(tokenizer_->tokenToString());
}

bool CompilationEngine::isStatementEnd() {
  return ((tokenizer_->getTokenType() == TokenType::SYMBOL) &&
          (tokenizer_->getSymbol() == ';'));
}
