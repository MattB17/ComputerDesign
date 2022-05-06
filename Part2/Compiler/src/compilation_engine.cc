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
  expectKeyword(Keyword::Type::VAR);
  // Write the `var` token.
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  //Expect a type and identifier pair (type and parameter).
  handleTypeAndIdentifierPair();

  // Check if we have reached the end of the statement. If not, we have a `,`
  // signifying additional variable names of the same type.
  while (!currentTokenIsExpectedSymbol(';')) {
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      // Expect a variable name and write its tag.
      expectIdentifier();
      writeTerminatedTokenAndTag();

      tokenizer_->nextToken();
    } else {
      throw ExpectedStatementEnd(tokenizer_->tokenToString());
    }
  }
  // Write the statement end `;`.
  writeTerminatedTokenAndTag();

  writeTerminatedCloseTag(var_tag);
  return;
}

void CompilationEngine::compileClassVarDec() {
  const std::string class_var_tag = "classVarDec";
  writeTerminatedOpenTag(class_var_tag);

  // Expects a class variable declaration and adds the appropriate tag.
  expectClassVarKeyword();
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();

  // Expects a type and identifier pair (type and parameter pair).
  handleTypeAndIdentifierPair();

  // Check if we have reached the end of the statement. If not, we have a `,`
  // signifying additional variable names of the same type.
  while (!currentTokenIsExpectedSymbol(';')) {
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      // Expect a variable name and write its tag.
      expectIdentifier();
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
    } else {
      throw ExpectedStatementEnd(tokenizer_->tokenToString());
    }
  }
  // Write the statement end `;`.
  writeTerminatedTokenAndTag();
  writeTerminatedCloseTag(class_var_tag);
  return;
}

void CompilationEngine::compileParameterList() {
  // It is assumed that the opening bracket is the first symbol so we advance
  // the tokenizer to inside the parameter list.
  tokenizer_->nextToken();

  const std::string parameter_list_tag = "parameterList";
  writeTerminatedOpenTag(parameter_list_tag);

  // We have an empty parameter list `()`.
  if (currentTokenIsExpectedSymbol(')')) {
    writeTerminatedCloseTag(parameter_list_tag);
    return;
  }

  // Expect a type and identifier (type and variable name) as the parameter
  // list is not empty.
  handleTypeAndIdentifierPair();

  // Check for more elements in the parameter list. Either we have hit the end
  // of the list (signified by `)`) or we get a `,` signifying more parameters.
  while (!currentTokenIsExpectedSymbol(')')) {
    // we have another parameter
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      // Expect a type and identifier (type and variable name).
      handleTypeAndIdentifierPair();
    } else {
      throw ExpectedClosingParenthesis(")", tokenizer_->tokenToString());
    }
  }
  writeTerminatedCloseTag(parameter_list_tag);
  return;
}

void CompilationEngine::compileReturn() {
  const std::string return_tag = "returnStatement";
  writeTerminatedOpenTag(return_tag);

  // write keyword return.
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  // we haven't hit statement end so we have the form `return expression;`
  if (!currentTokenIsExpectedSymbol(';')) {
    compileExpression();
  }

  tokenizer_->nextToken();

  // now we expect statement end.
  if (!currentTokenIsExpectedSymbol(';')) {
    throw ExpectedStatementEnd(tokenizer_->tokenToString());
  }
  writeTerminatedTokenAndTag();

  writeTerminatedCloseTag(return_tag);
  return;
}

// Let statements are of 2 forms `let varName = expression;` or
// `let varName[expression] = expression;`.
void CompilationEngine::compileLet() {
  const std::string let_tag = "letStatement";
  writeTerminatedOpenTag(let_tag);

  // write keyword let.
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  // expect a valid identifier for the varname
  expectIdentifier();
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  // If we encounter `[` then we know we are in the second case.
  if (currentTokenIsExpectedSymbol('[')) {
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();
    compileExpression();
    tokenizer_->nextToken();
    if (currentTokenIsExpectedSymbol(']')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
    } else {
      throw ExpectedSymbol(tokenizer_->tokenToString(), "]", let_tag);
    }
  }

  // handle the equal sign
  if (currentTokenIsExpectedSymbol('=')) {
    writeTerminatedTokenAndTag();
  } else {
    throw ExpectedSymbol(tokenizer_->tokenToString(), "=", let_tag);
  }
  tokenizer_->nextToken();

  // handle the expression of the right of the `=`.
  compileExpression();

  tokenizer_->nextToken();

  // now we expect statement end.
  if (!currentTokenIsExpectedSymbol(';')) {
    throw ExpectedStatementEnd(tokenizer_->tokenToString());
  }
  writeTerminatedTokenAndTag();

  writeTerminatedCloseTag(let_tag);
  return;
}

void CompilationEngine::compileTerm() {
  const std::string term_tag = "term";
  writeTerminatedOpenTag(term_tag);
  expectTerm();

  // Write the term and its appropriate tags.
  writeTerminatedTokenAndTag();

  writeTerminatedCloseTag(term_tag);
  return;
}

void CompilationEngine::compileExpression() {
  const std::string expression_tag = "expression";
  writeTerminatedOpenTag(expression_tag);
  compileTerm();
  writeTerminatedCloseTag(expression_tag);
  return;
}

void CompilationEngine::compileExpressionList() {
  tokenizer_->nextToken();
  // It is assumed that the opening bracket is the first symbol so we advance
  // the tokenizer to inside the parameter list.
  tokenizer_->nextToken();

  const std::string expression_list_tag = "expressionList";
  writeTerminatedOpenTag(expression_list_tag);

  // We have an empty parameter list `()`.
  if (currentTokenIsExpectedSymbol(')')) {
    writeTerminatedCloseTag(expression_list_tag);
    return;
  }

  // expect an expression.
  compileExpression();

  tokenizer_->nextToken();

  // Check for more elements in the expression list. Either we have hit the end
  // of the list (signified by `)`) or we get a `,` signifying more parameters.
  while (!currentTokenIsExpectedSymbol(')')) {
    // we have another expression.
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      // Expect an expression.
      compileExpression();
      tokenizer_->nextToken();
    } else {
      throw ExpectedClosingParenthesis(")", tokenizer_->tokenToString());
    }
  }
  writeTerminatedCloseTag(expression_list_tag);
  return;
}

void CompilationEngine::writeTokenWithTag() {
  std::string tag;
  switch (tokenizer_->getTokenType()) {
    case TokenType::KEYWORD:
      tag = "keyword";
      writeOpenTag(tag);
      xml_stream_ << " " << Keyword::KeywordToString(tokenizer_->getKeyword());
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
  writeTabs();
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

void CompilationEngine::expectKeyword(Keyword::Type k) {
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
      (Keyword::IsPrimitiveType(tokenizer_->getKeyword()))) {
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

void CompilationEngine::expectTerm() {
  if ((tokenizer_->getTokenType() == TokenType::STRING_CONST) ||
      (tokenizer_->getTokenType() == TokenType::INT_CONST) ||
      (tokenizer_->getTokenType() == TokenType::IDENTIFIER) ||
      ((tokenizer_->getTokenType() == TokenType::KEYWORD) &&
       (Keyword::IsKeywordConstant(tokenizer_->getKeyword())))) {
    return;
  }
  throw InvalidTerm(tokenizer_->tokenToString());
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
      ((tokenizer_->getKeyword() == Keyword::Type::STATIC) ||
       (tokenizer_->getKeyword() == Keyword::Type::FIELD))) {
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
