#include "compilation_engine.h"

#include <iostream>
#include <sstream>
#include <string>

#include "exceptions.h"
#include "segment.h"
#include "util.h"

CompilationEngine::CompilationEngine() : num_spaces_(0) {}

void CompilationEngine::compile(std::string jack_file) {
  setJackFile(jack_file);
  bool moreClasses = tokenizer_->nextToken();
  while (moreClasses) {
    compileClass();
    moreClasses = (tokenizer_->getTokenType() != TokenType::UNKNOWN);
  }
  xml_stream_.close();
}

void CompilationEngine::compileClass() {
  scope_list_ = std::make_unique<ScopeList>();

  const std::string class_tag = "class";
  writeTerminatedOpenTag(class_tag);

  expectKeyword(Keyword::Type::CLASS);
  writeTerminatedTokenAndTag();

  tokenizer_->nextToken();

  // expect an identifier for the class name.
  expectIdentifier();
  curr_class_ = tokenizer_->tokenToString();
  writeTerminatedTagForToken(/*tag=*/"className", /*class_name=*/curr_class_);
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
  Segment var_segment = Segment::LOCAL;

  const std::string var_tag = "varDec";
  writeTerminatedOpenTag(var_tag);

  // Write the `var` token. We know this is the current token as this is the
  // precondition for entering this method.
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();

  // Retrieve the variable type and define the variable.
  std::string var_type = getType();
  handleVariableDefinition(var_type, var_segment);

  // Check if we have reached the end of the statement. If not, we have a `,`
  // signifying additional variable names of the same type.
  compileAdditionalVarDecs(var_type, var_segment, var_tag);

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
  Segment var_segment = GetSegmentFromClassVarKeyword(tokenizer_->getKeyword());
  tokenizer_->nextToken();

  // Retrieve the variable type and define the variable.
  std::string var_type = getType();
  handleVariableDefinition(var_type, var_segment);

  // Check if we have reached the end of the statement. If not, we have a `,`
  // signifying additional variable names of the same type.
  compileAdditionalVarDecs(var_type, var_segment, class_var_tag);

  handleStatementEnd(class_var_tag);

  writeTerminatedCloseTag(class_var_tag);
  return;
}

void CompilationEngine::compileParameterList() {
  Segment var_segment = Segment::ARGUMENT;

  const std::string parameter_list_tag = "parameterList";
  writeTerminatedOpenTag(parameter_list_tag);

  // We have an empty parameter list `()`.
  if (currentTokenIsExpectedSymbol(')')) {
    writeTerminatedCloseTag(parameter_list_tag);
    return;
  }

  // Get variable type and name as the parameter list is not empty.
  std::string var_type = getType();
  handleVariableDefinition(var_type, var_segment);

  // Check for more elements in the parameter list. Either we have hit the end
  // of the list (signified by `)`) or we get a `,` signifying more parameters.
  while (!currentTokenIsExpectedSymbol(')')) {
    // we have another parameter
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      // Get type and name of next variable in the parameter list.
      var_type = getType();
      handleVariableDefinition(var_type, var_segment);
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

  // expect a valid identifier for the variable name.
  expectIdentifier();
  writeTerminatedVarTag(
    /*var_name=*/tokenizer_->tokenToString(), /*expect_definition=*/true);
  tokenizer_->nextToken();

  // If we encounter `[` then we know we are in the second case.
  if (currentTokenIsExpectedSymbol('[')) {
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();
    compileExpression();
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
    } else if (tokenizer_->getSymbol() == '(') {
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
    std::string identifier_name = tokenizer_->tokenToString();
    tokenizer_->nextToken();

    if (currentTokenIsExpectedSymbol('[')) {
      // In this case we have an array element. Start by writing the array
      // variable name, then handle the array index.
      writeTerminatedVarTag(
        /*var_name=*/identifier_name, /*expect_definition=*/true);
      handleOpeningParenthesis('[', term_tag);
      compileExpression();
      handleClosingParenthesis(']', term_tag);
    } else if (currentTokenIsExpectedSymbol('(')) {
      // We have a subroutine call of the type `methodName(expressionList)`.
      // So we write the subroutine name and then proceed with handling the
      // brackets and expression list.
      writeTerminatedVarTag(/*var_name=*/identifier_name,
                            /*expect_definition=*/false,
                            /*default_is_class=*/false);
      handleOpeningParenthesis('(', term_tag);
      compileExpressionList();
      handleClosingParenthesis(')', term_tag);
    } else if (currentTokenIsExpectedSymbol('.')) {
      // We have a subroutine call of the type
      // `className.methodName(expressionList)`. We write the class name first
      // and then we write the `.`. Then note that `methodName(expressionList)`
      // has the form of a subroutine call.
      writeTerminatedVarTag(/*var_name=*/identifier_name,
                            /*expect_definition=*/false,
                            /*default_is_class=*/true);
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();
      compileSubroutineCall();
    } else {
      // it's just a simple identifier for a variable name.
      writeTerminatedVarTag(
        /*var_name=*/identifier_name, /*expect_definition=*/true);
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
  scope_list_->startSubroutine();

  const std::string subroutine_tag = "subroutineDec";
  writeTerminatedOpenTag(subroutine_tag);

  expectSubroutineDecKeyword();
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();

  expectFunctionReturnType();
  // We have a valid function return type, so it is either a primitive type,
  // void, or a class name. That is, an identifier or keyword.
  if (tokenizer_->getTokenType() == TokenType::IDENTIFIER) {
    // We have a class name.
    writeTerminatedTagForToken(
      /*tag=*/"className", /*token=*/tokenizer_->tokenToString());
  } else {
    // We have a primitive type or void, so just compile the the keyword.
    writeTerminatedTokenAndTag();
  }
  tokenizer_->nextToken();

  // expect a valid identifier for the subroutine name.
  expectIdentifier();
  writeTerminatedTagForToken(
    /*tag=*/"subroutineName", /*token=*/tokenizer_->tokenToString());
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

void CompilationEngine::setJackFile(std::string jack_file) {
  tokenizer_ = std::make_unique<Tokenizer>(jack_file);
  num_spaces_ = 0;
  xml_stream_.open(jackFileToXmlFile(jack_file));
}

// A subroutine call has one of 2 forms: `subroutineName(expressionList)` or
// `className.subroutineName(expressionList)`.
void CompilationEngine::compileSubroutineCall() {
  const std::string call_tag = "subroutineCall";
  // expect an identifier (either `subroutineName` or `varName`).
  expectIdentifier();
  std::string identifier_name = tokenizer_->tokenToString();
  tokenizer_->nextToken();

  // This means we are in the second case. So we write out the class tag and
  // then compile the rest. Note that the part after `.` also has the form of
  // a subroutine call so we can recurse.
  if (currentTokenIsExpectedSymbol('.')) {
    // Write className / varName first.
    writeTerminatedVarTag(/*var_name=*/identifier_name,
                          /*expect_definition=*/false,
                          /*default_is_class=*/true);

    // Then write the `.`.
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();

    // Then recurse.
    compileSubroutineCall();
    return;
  }

  // Otherwise, we have a plain subroutine call. So we write the subroutine name
  // and then compile the expression list.
  writeTerminatedVarTag(/*var_name=*/identifier_name,
                        /*expect_definition=*/false,
                        /*default_is_class=*/false);

  // Now we expect the start of the expression list.
  handleOpeningParenthesis('(', call_tag);
  compileExpressionList();
  handleClosingParenthesis(')', call_tag);

  return;
}

void CompilationEngine::compileAdditionalVarDecs(
  std::string var_type, Segment var_segment, const std::string compile_tag) {
  while (!currentTokenIsExpectedSymbol(';')) {
    if (currentTokenIsExpectedSymbol(',')) {
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      handleVariableDefinition(var_type, var_segment);
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
      // If the keyword is `this` and it is in the symbol table then we are
      // in a method and so `this` is a variable, not a keyword.
      if (tokenizer_->getKeyword() == Keyword::Type::THIS) {
        std::string var_name = tokenizer_->tokenToString();
        SymbolData var_data = scope_list_->getVarData(var_name);
        if (var_data.segment != Segment::NONE) {
          handleVarOutput(var_name, var_data);
          return;
        }
      }
      // Otherwise, just a normal keyword.
      tag = "keyword";
      break;
    case TokenType::SYMBOL:
      tag = "symbol";
      break;
    case TokenType::IDENTIFIER:
      writeVarTag(
        /*var_name=*/tokenizer_->tokenToString(), /*expect_definition=*/true);
      return;
    case TokenType::INT_CONST:
      tag = "integerConstant";
      break;
    case TokenType::STRING_CONST:
      tag = "stringConstant";
      break;
    default:
      tag = "unknown";
  }
  writeTagForToken(tag, tokenizer_->tokenToString());
}

void CompilationEngine::writeTagForToken(std::string tag, std::string token) {
  writeOpenTag(tag);
  std::cout << token << std::endl;
  xml_stream_ << " " << token << " ";
  writeCloseTag(tag);
}

void CompilationEngine::writeOpenTag(const std::string tag) {
  xml_stream_ << "<" << tag << ">";
}

void CompilationEngine::writeCloseTag(const std::string tag) {
  xml_stream_ << "</" << tag << ">";
}

void CompilationEngine::writeTerminatedTagForToken(
  std::string tag, std::string token) {
  writeSpaces();
  writeTagForToken(tag, token);
  xml_stream_ << '\n';
}

void CompilationEngine::writeTerminatedVarTag(
  std::string var_name, bool expect_definition, bool default_is_class) {
  writeSpaces();
  writeVarTag(var_name, expect_definition, default_is_class);
  xml_stream_ << '\n';
}

void CompilationEngine::writeVarTag(
  std::string var_name, bool expect_definition, bool default_is_class) {
  SymbolData var_data = scope_list_->getVarData(var_name);
  if (var_data.segment == Segment::NONE) {
    if (expect_definition) {
      throw UndeclaredVariable(var_name);
    }
    // Otherwise, we don't expect it to be defined. As it is not defined, it
    // is either a class name or subroutine name.
    if (default_is_class) {
      writeTagForToken(/*tag=*/"className", /*token=*/var_name);
    } else {
      writeTagForToken(/*tag=*/"subroutineName", /*token=*/var_name);
    }
    return;
  }
  handleVarOutput(var_name, var_data);
}

void CompilationEngine::handleVarOutput(
  std::string var_name, SymbolData var_data) {
  std::stringstream ss;
  ss << var_name << " " << var_data.offset;
  writeTagForToken(
    /*tag=*/SegmentToString(var_data.segment), /*token=*/ss.str());
}

void CompilationEngine::writeTerminatedTokenAndTag() {
  writeSpaces();
  writeTokenWithTag();
  xml_stream_ << '\n';
}

void CompilationEngine::writeTerminatedOpenTag(const std::string tag) {
  writeSpaces();
  writeOpenTag(tag);
  xml_stream_ << '\n';
  num_spaces_ = num_spaces_ + 2;
}

void CompilationEngine::writeTerminatedCloseTag(const std::string tag) {
  num_spaces_ = num_spaces_ - 2;
  writeSpaces();
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
        (tokenizer_->getKeyword() == Keyword::Type::FUNCTION)) {
      return;
    } else if (tokenizer_->getKeyword() == Keyword::Type::METHOD) {
      // Add this as the first argument to the subroutine symbol table.
      scope_list_->define(/*var_name=*/"this",
                          /*var_type=*/curr_class_,
                          /*var_segment=*/Segment::ARGUMENT);
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

void CompilationEngine::handleVariableDefinition(
  std::string var_type, Segment var_segment) {
  std::string var_name = getVarName();
  scope_list_->define(var_name, var_type, var_segment);
  writeTerminatedVarTag(var_name, /*expect_definition=*/true);
}

std::string CompilationEngine::getType() {
  expectType();
  std::string var_type = tokenizer_->tokenToString();
  // The type is either a simple type or a class.
  if (tokenizer_->getTokenType() == TokenType::KEYWORD) {
    // we have a simple type so just compile the type.
    writeTerminatedTokenAndTag();
  } else {
    // We have already validated the type so it must be a class.
    writeTerminatedTagForToken(/*tag=*/"className", /*token=*/var_type);
  }
  tokenizer_->nextToken();
  return var_type;
}

std::string CompilationEngine::getVarName() {
  expectIdentifier();
  std::string var_name = tokenizer_->tokenToString();
  tokenizer_->nextToken();
  return var_name;
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

void CompilationEngine::writeSpaces() {
  for (int i = 0; i < num_spaces_; i++) {
    xml_stream_ << ' ';
  }
}
