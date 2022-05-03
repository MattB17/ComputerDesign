#include "compilation_engine.h"

#include <iostream>
#include <string>

#include "exceptions.h"
#include "util.h"

CompilationEngine::CompilationEngine(std::string jack_file)
  : tokenizer_(std::make_unique<Tokenizer>(jack_file)), num_tabs_(0)
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
  const std::string var_tag = "varDec";
  writeTerminatedOpenTag(var_tag);
  expectKeyword(Keyword::VAR);
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();
  handleTypeAndIdentifierPair();
  while (!currentTokenIsExpectedSymbol(';')) {
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
      expectIdentifier();
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
    } else {
      throw ExpectedStatementEnd(tokenizer_->tokenToString());
    }
  }
  writeTerminatedTokenAndTag();
  writeTerminatedCloseTag(var_tag);
}

void CompilationEngine::compileClassVarDec() {
  const std::string class_var_tag = "classVarDec";
  writeTerminatedOpenTag(class_var_tag);
  expectClassVarKeyword();
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();
  handleTypeAndIdentifierPair();
  while (!currentTokenIsExpectedSymbol(';')) {
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
      expectIdentifier();
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
    } else {
      throw ExpectedStatementEnd(tokenizer_->tokenToString());
    }
  }
  writeTerminatedTokenAndTag();
  writeTerminatedCloseTag(class_var_tag);
}

void CompilationEngine::compileParameterList() {
  const std::string parameter_list_tag = "parameterList";
  writeTerminatedOpenTag(parameter_list_tag);
  tokenizer_->nextToken();
  if (currentTokenIsExpectedSymbol(')')) {
    writeTerminatedCloseTag(parameter_list_tag);
    return;
  }
  handleTypeAndIdentifierPair();
  while (!currentTokenIsExpectedSymbol(')')) {
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
      handleTypeAndIdentifierPair();
    } else {
      throw ExpectedClosingParenthesis(")", tokenizer_->tokenToString());
    }
  }
  writeTerminatedCloseTag(parameter_list_tag);
}

void CompilationEngine::writeTokenWithTag() {
  writeTabs();
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

void CompilationEngine::writeTerminatedTokenAndTag() {
  writeTokenWithTag();
  xml_stream_ << '\n';
}

void CompilationEngine::writeTerminatedOpenTag(std::string tag) {
  writeTabs();
  writeOpenTag(tag);
  xml_stream_ << '\n';
  num_tabs_++;
}

void CompilationEngine::writeTerminatedCloseTag(std::string tag) {
  num_tabs_--;
  writeTabs();
  writeCloseTag(tag);
  xml_stream_ << '\n';
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

void CompilationEngine::handleTypeAndIdentifierPair() {
  expectType();
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();
  expectIdentifier();
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();
}

void CompilationEngine::expectClassVarKeyword() {
  if ((tokenizer_->getTokenType() == TokenType::KEYWORD) &&
      ((tokenizer_->getKeyword() == Keyword::STATIC) ||
       (tokenizer_->getKeyword() == Keyword::FIELD))) {
    return;
  }
  throw InvalidClassVarKeyword(tokenizer_->tokenToString());
}

bool CompilationEngine::currentTokenIsExpectedSymbol(char expected_symbol) {
  return ((tokenizer_->getTokenType() == TokenType::SYMBOL) &&
          (tokenizer_->getSymbol() == expected_symbol));
}

void CompilationEngine::writeTabs() {
  for (int i = 0; i < num_tabs_; i++) {
    xml_stream_ << '\t';
  }
}
