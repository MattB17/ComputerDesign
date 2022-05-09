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
  bool moreClasses = tokenizer_->nextToken();
  while (moreClasses) {
    compileClass();
    moreClasses = (tokenizer_->getTokenType() != TokenType::UNKNOWN);
  }
}

void CompilationEngine::compileClass() {
  const std::string class_tag = "class";
  writeTerminatedOpenTag(class_tag);

  expectKeyword(Keyword::Type::CLASS);
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  // expect an identifier for the class name.
  expectIdentifier();
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  // opening bracket to start the class definition.
  handleOpeningParenthesis('{', class_tag);

  // now we expect a sequence of class var declaration statements.
  while (currentTokenIsClassVarKeyword()) {
    compileClassVarDec();
    tokenizer_->nextToken();
  }

  // then we expect a sequence of subroutine declarations. Everything from now
  // until the closing `}` should be part of a subroutine declaration.
  while (!currentTokenIsExpectedSymbol('}')) {
    compileSubroutineDec();
  }

  // lastly we compile the closing parenthesis
  handleClosingParenthesis('}', class_tag);

  writeTerminatedCloseTag(class_tag);
  return;
}

void CompilationEngine::compileVarDec() {
  const std::string var_tag = "varDec";
  writeTerminatedOpenTag(var_tag);

  // Write the `var` token. We know this is the current token as this is the
  // precondition for entering this method.
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

  // Adds a tag for a class variable declaration. We already know it is a class
  // variable declaration keyword because we check for that before entering
  // this function.
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();

  // Expects a type and identifier pair (type and parameter pair).
  handleTypeAndIdentifierPair();

  // Check if we have reached the end of the statement. If not, we have a `,`
  // signifying additional variable names of the same type.
  compileAdditionalVarDecs(class_var_tag);

  handleStatementEnd(class_var_tag);

  writeTerminatedCloseTag(class_var_tag);
  return;
}

void CompilationEngine::compileParameterList() {
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
    tokenizer_->nextToken();
  }

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

  if (tokenizer_->getTokenType() == TokenType::SYMBOL) {
    if (IsUnaryOp(tokenizer_->getSymbol())) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
      compileTerm();
    } else if (tokenizer_->getSymbol() = '(') {
      handleOpeningParenthesis('(', term_tag);
      compileExpression();
      handleClosingParenthesis(')', term_tag);
    } else {
      throw InvalidTerm(tokenizer_->tokenToString());
    }
  } else if (currentTokenIsSimpleTerm()) {
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();
  } else {
    // we must have an identifier. Either a simple variable name, an array
    // element, or a subroutine call.
    expectIdentifier();
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();

    // After here, if it is just a variable name then none of the conditions
    // are true, and we just skip to the end.

    if (currentTokenIsExpectedSymbol('[')) {
      // In this case we have an array element.
      handleOpeningParenthesis('[', term_tag);
      compileExpression();
      handleClosingParenthesis(']', term_tag);
    } else if (currentTokenIsExpectedSymbol('(')) {
      // We have a subroutine call of the type `methodName(expressionList)`.
      handleOpeningParenthesis('(', term_tag);
      compileExpressionList();
      handleClosingParenthesis(')', term_tag);
    } else if (currentTokenIsExpectedSymbol('.')) {
      // We have a subroutine call of the type
      // `className.methodName(expressionList)`. In this case we write the `.`
      // but then note that `methodName(expressionList)` is a subroutine call.
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
      compileSubroutineCall();
    }
  }

  writeTerminatedCloseTag(term_tag);
  return;
}

void CompilationEngine::compileExpression() {
  const std::string expression_tag = "expression";
  writeTerminatedOpenTag(expression_tag);

  // We start with a term.
  compileTerm();

  // Then we check for a binary op to tell us there are more terms in the
  // expression.
  while (currentTokenIsBinaryOp()) {
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();
    compileTerm();
  }

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

  // Check for more elements in the expression list. Either we have hit the end
  // of the list (signified by `)`) or we get a `,` signifying more parameters.
  while (!currentTokenIsExpectedSymbol(')')) {
    // we have another expression.
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      // Expect an expression.
      compileExpression();
    } else {
      throw ExpectedClosingParenthesis(
        tokenizer_->tokenToString(), ")", expression_list_tag);
    }
  }
  writeTerminatedCloseTag(expression_list_tag);
  return;
}

void CompilationEngine::compileSubroutineDec() {
  const std::string subroutine_tag = "subroutineDec";
  writeTerminatedOpenTag(subroutine_tag);

  expectSubroutineDecKeyword();
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();

  expectFunctionReturnType();
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();

  // expect a valid identifier for the subroutine name.
  expectIdentifier();
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();

  // Now we expect a parameter list enclosed in `(` and `)`.
  handleOpeningParenthesis('(', subroutine_tag);
  compileParameterList();
  handleClosingParenthesis(')', subroutine_tag);

  // Lastly we compile the body of the subroutine.
  compileSubroutineBody();

  writeTerminatedCloseTag(subroutine_tag);
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

  if (currentTokenIsStatementKeyword()) {
    compileStatements();
  }

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
  while (currentTokenIsExpectedSymbol('.')) {
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();

    compileSubroutineCall();
  }

  // Now we expect the start of the expression list.
  handleOpeningParenthesis('(', call_tag);
  compileExpressionList();
  handleClosingParenthesis(')', call_tag);

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

void CompilationEngine::expectFunctionReturnType() {
  if (tokenizer_->getTokenType() == TokenType::IDENTIFIER) {
    return;
  }
  if (tokenizer_->getTokenType() == TokenType::KEYWORD) {
    if ((Keyword::IsPrimitiveType(tokenizer_->getKeyword())) ||
        (tokenizer_->getKeyword() == Keyword::Type::VOID)) {
      return;
    }
  }
  throw InvalidFunctionReturnType(tokenizer_->tokenToString());
}

void CompilationEngine::expectIdentifier() {
  if (tokenizer_->getTokenType() == TokenType::IDENTIFIER) {
    return;
  }
  throw MissingIdentifier(tokenizer_->tokenToString());
}

bool CompilationEngine::currentTokenIsSimpleTerm() {
  if ((tokenizer_->getTokenType() == TokenType::STRING_CONST) ||
      (tokenizer_->getTokenType() == TokenType::INT_CONST) ||
      ((tokenizer_->getTokenType() == TokenType::KEYWORD) &&
       (Keyword::IsKeywordConstant(tokenizer_->getKeyword())))) {
    return true;
  }
  return false;
}

void CompilationEngine::expectSubroutineDecKeyword() {
  if (tokenizer_->getTokenType() == TokenType::KEYWORD) {
    if ((tokenizer_->getKeyword() == Keyword::Type::CONSTRUCTOR) ||
        (tokenizer_->getKeyword() == Keyword::Type::FUNCTION) ||
        (tokenizer_->getKeyword() == Keyword::Type::METHOD)) {
      return;
    }
  }
  throw InvalidSubroutineDecKeyword(tokenizer_->tokenToString());
}

bool CompilationEngine::currentTokenIsBinaryOp() {
  if ((tokenizer_->getTokenType() == TokenType::SYMBOL) &&
      (IsBinaryOp(tokenizer_->getSymbol()))) {
    return true;
  }
  return false;
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

bool CompilationEngine::currentTokenIsClassVarKeyword() {
  if (tokenizer_->getTokenType() == TokenType::KEYWORD) {
    if ((tokenizer_->getKeyword() == Keyword::Type::STATIC) ||
        (tokenizer_->getKeyword() == Keyword::Type::FIELD)) {
      return true;
    }
  }
  return false;
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
