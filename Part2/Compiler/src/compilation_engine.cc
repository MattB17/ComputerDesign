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

void CompilationEngine::compileTemp() {
  tokenizer_->nextToken();
  compileStatements();
  return;
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
  compileAdditionalVarDecs(var_tag);

  handleStatementEnd(var_tag);

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
  compileAdditionalVarDecs(class_var_tag);

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
      throw ExpectedClosingParenthesis(
        tokenizer_->tokenToString(), ")", parameter_list_tag);
    }
  }
  writeTerminatedCloseTag(parameter_list_tag);
  return;
}

void CompilationEngine::compileStatements() {
  const std::string statements_tag = "statements";
  writeTerminatedOpenTag(statements_tag);

  // loop through statements, compiling each one.
  while (currentTokenIsStatementKeyword()) {
    switch (tokenizer_->getKeyword()) {
      case Keyword::Type::LET:
        compileLet();
        tokenizer_->nextToken();
        break;
      case Keyword::Type::IF:
        compileIf();
        break;
      case Keyword::Type::WHILE:
        compileWhile();
        break;
      case Keyword::Type::DO:
        compileDo();
        tokenizer_->nextToken();
        break;
      default:
        compileReturn();
        tokenizer_->nextToken();
    }
  }
  writeTerminatedCloseTag(statements_tag);
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
  handleStatementEnd(return_tag);

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
    handleClosingParenthesis(']', let_tag);
  }

  // handle the equal sign
  if (currentTokenIsExpectedSymbol('=')) {
    writeTerminatedTokenAndTag();
  } else {
    throw ExpectedSymbol(tokenizer_->tokenToString(), "=", let_tag);
  }
  tokenizer_->nextToken();

  // handle the expression on the right of the `=`.
  compileExpression();

  tokenizer_->nextToken();

  // now we expect statement end.
  handleStatementEnd(let_tag);

  writeTerminatedCloseTag(let_tag);
  return;
}

void CompilationEngine::compileIf() {
  const std::string if_tag = "ifStatement";
  writeTerminatedOpenTag(if_tag);

  // write keyword if.
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  compileStatementCondition(if_tag);
  compileScopedStatements(if_tag);

  if (currentTokenIsExpectedKeyword(Keyword::Type::ELSE)) {
    writeTerminatedTokenAndTag();

    tokenizer_->nextToken();
    compileScopedStatements("elseStatement");
  }

  writeTerminatedCloseTag(if_tag);
  return;
}

void CompilationEngine::compileWhile() {
  const std::string while_tag = "whileStatement";
  writeTerminatedOpenTag(while_tag);

  // write keyword while.
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  compileStatementCondition(while_tag);
  compileScopedStatements(while_tag);

  writeTerminatedCloseTag(while_tag);
  return;
}

void CompilationEngine::compileDo() {
  const std::string do_tag = "doStatement";
  writeTerminatedOpenTag(do_tag);

  // write keyword do.
  writeTerminatedTokenAndTag();

  // compile the subroutine call.
  tokenizer_->nextToken();
  compileSubroutineCall();

  tokenizer_->nextToken();

  // Expect end of statement.
  handleStatementEnd(do_tag);

  writeTerminatedCloseTag(do_tag);
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
      throw ExpectedClosingParenthesis(
        tokenizer_->tokenToString(), ")", expression_list_tag);
    }
  }
  writeTerminatedCloseTag(expression_list_tag);
  return;
}

void CompilationEngine::compileSubroutineBody() {
  const std::string subroutine_tag = "subroutineBody";
  writeTerminatedOpenTag(subroutine_tag);

  handleOpeningParenthesis('{', subroutine_tag);

  while (currentTokenIsExpectedKeyword(Keyword::Type::VAR)) {
    compileVarDec();
    tokenizer_->nextToken();
  }
  compileStatements();

  handleClosingParenthesis('}', subroutine_tag);

  writeTerminatedCloseTag(subroutine_tag);
  return;
}

// A subroutine call has one of 2 forms: `subroutineName(expressionList)` or
// `varName.subroutineName(expressionList)`.
void CompilationEngine::compileSubroutineCall() {
  const std::string call_tag = "subroutineCall";
  // expect an identifier (either `subroutineName` or `varName`).
  expectIdentifier();
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  // this means we are in the second case.
  if (currentTokenIsExpectedSymbol('.')) {
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();

    // expect an identifier for the subroutine
    expectIdentifier();
    writeTerminatedTokenAndTag();

    tokenizer_->nextToken();
  }

  // Now we expect the start of the expression list.
  if (!currentTokenIsExpectedSymbol('(')) {
    throw ExpectedOpeningParenthesis(
      tokenizer_->tokenToString(), "(", call_tag);
  }
  writeTerminatedTokenAndTag();
  compileExpressionList();

  // Now we expect the end of the expression list.
  if (!currentTokenIsExpectedSymbol(')')) {
    throw ExpectedClosingParenthesis(
      tokenizer_->tokenToString(), ")", call_tag);
  }
  writeTerminatedTokenAndTag();
  return;
}

void CompilationEngine::compileAdditionalVarDecs(
  const std::string compile_tag) {
  while (!currentTokenIsExpectedSymbol(';')) {
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      // Expect a variable name and write its tag.
      expectIdentifier();
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
    } else {
      throw ExpectedStatementEnd(tokenizer_->tokenToString(), compile_tag);
    }
  }
  return;
}

void CompilationEngine::compileStatementCondition(
  const std::string compile_tag) {
  handleOpeningParenthesis('(', compile_tag);
  compileExpression();
  tokenizer_->nextToken();
  handleClosingParenthesis(')', compile_tag);
  return;
}

void CompilationEngine::compileScopedStatements(const std::string compile_tag) {
  handleOpeningParenthesis('{', compile_tag);
  compileStatements();
  handleClosingParenthesis('}', compile_tag);
  return;
}

void CompilationEngine::writeTokenWithTag() {
  std::string tag;
  switch (tokenizer_->getTokenType()) {
    case TokenType::KEYWORD:
      tag = "keyword";
      break;
    case TokenType::SYMBOL:
      tag = "symbol";
      break;
    case TokenType::IDENTIFIER:
      tag = "identifier";
      break;
    case TokenType::INT_CONST:
      tag = "intConstant";
      break;
    case TokenType::STRING_CONST:
      tag = "stringConstant";
      break;
    default:
      tag = "unknown";
  }
  writeOpenTag(tag);
  xml_stream_ << " " << tokenizer_->tokenToString() << " ";
  writeCloseTag(tag);
}

void CompilationEngine::writeOpenTag(const std::string tag) {
  xml_stream_ << "<" << tag << ">";
}

void CompilationEngine::writeCloseTag(const std::string tag) {
  xml_stream_ << "</" << tag << ">";
}

void CompilationEngine::writeTerminatedTokenAndTag() {
  writeTabs();
  writeTokenWithTag();
  xml_stream_ << '\n';
}

void CompilationEngine::writeTerminatedOpenTag(const std::string tag) {
  writeTabs();
  writeOpenTag(tag);
  xml_stream_ << '\n';
  num_tabs_++;
}

void CompilationEngine::writeTerminatedCloseTag(const std::string tag) {
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

void CompilationEngine::handleStatementEnd(const std::string compile_tag) {
  if (!currentTokenIsExpectedSymbol(';')) {
    throw ExpectedStatementEnd(tokenizer_->tokenToString(), compile_tag);
  }
  writeTerminatedTokenAndTag();
  return;
}

void CompilationEngine::handleOpeningParenthesis(
  char parenthesis, const std::string compile_tag) {
  if (currentTokenIsExpectedSymbol(parenthesis)) {
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();
  } else {
    throw ExpectedOpeningParenthesis(
      tokenizer_->tokenToString(), std::string(1, parenthesis), compile_tag);
  }
  return;
}

void CompilationEngine::handleClosingParenthesis(
  char parenthesis, const std::string compile_tag) {
  if (currentTokenIsExpectedSymbol(parenthesis)) {
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();
  } else {
    throw ExpectedClosingParenthesis(
      tokenizer_->tokenToString(), std::string(1, parenthesis), compile_tag);
  }
  return;
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

bool CompilationEngine::currentTokenIsExpectedKeyword(Keyword::Type k) {
  return ((tokenizer_->getTokenType() == TokenType::KEYWORD) &&
          (tokenizer_->getKeyword() == k));
}

bool CompilationEngine::currentTokenIsStatementKeyword() {
  if (tokenizer_->getTokenType() == TokenType::KEYWORD) {
    if ((tokenizer_->getKeyword() == Keyword::Type::LET) ||
        (tokenizer_->getKeyword() == Keyword::Type::IF) ||
        (tokenizer_->getKeyword() == Keyword::Type::WHILE) ||
        (tokenizer_->getKeyword() == Keyword::Type::DO) ||
        (tokenizer_->getKeyword() == Keyword::Type::RETURN)) {
      return true;
    }
  }
  return false;
}

void CompilationEngine::writeTabs() {
  for (int i = 0; i < num_tabs_; i++) {
    xml_stream_ << '\t';
  }
}
