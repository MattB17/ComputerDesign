#include "compilation_engine.h"

#include <iostream>
#include <sstream>
#include <string>

#include "exceptions.h"
#include "segment.h"
#include "util.h"

void CompilationEngine::compile(std::string jack_file) {
  setJackFile(jack_file);
  bool moreClasses = tokenizer_->nextToken();
  while (moreClasses) {
    compileClass();
    moreClasses = (tokenizer_->getTokenType() != TokenType::UNKNOWN);
  }
  vm_writer_->close();
}

void CompilationEngine::compileClass() {
  scope_list_ = std::make_unique<ScopeList>();

  const std::string class_tag = "class";

  expectKeyword(Keyword::Type::CLASS);
  tokenizer_->nextToken();

  // expect an identifier for the class name.
  expectIdentifier();
  curr_class_ = tokenizer_->tokenToString();
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
  return;
}

void CompilationEngine::compileVarDec() {
  Segment var_segment = Segment::LOCAL;

  const std::string var_tag = "varDec";

  // We know `var` is the current token as this is the precondition for
  // entering this method. So just advance past it.
  tokenizer_->nextToken();

  // Retrieve the variable type and define the variable.
  std::string var_type = getType();
  handleVariableDefinition(var_type, var_segment);

  // Check if we have reached the end of the statement. If not, we have a `,`
  // signifying additional variable names of the same type.
  compileAdditionalVarDecs(var_type, var_segment, var_tag);

  handleStatementEnd(var_tag);
  return;
}

void CompilationEngine::compileClassVarDec() {
  const std::string class_var_tag = "classVarDec";

  // We already know it is a class variable declaration keyword because we check
  // for that before entering this function.
  Segment var_segment = GetSegmentFromClassVarKeyword(tokenizer_->getKeyword());
  tokenizer_->nextToken();

  // Retrieve the variable type and define the variable.
  std::string var_type = getType();
  handleVariableDefinition(var_type, var_segment);

  // Check if we have reached the end of the statement. If not, we have a `,`
  // signifying additional variable names of the same type.
  compileAdditionalVarDecs(var_type, var_segment, class_var_tag);

  handleStatementEnd(class_var_tag);
  return;
}

void CompilationEngine::compileParameterList() {
  Segment var_segment = Segment::ARGUMENT;
  const std::string parameter_list_tag = "parameterList";

  // We have an empty parameter list `()`.
  if (currentTokenIsExpectedSymbol(')')) {
    return;
  }

  // Get variable type and name as the parameter list is not empty.
  std::string var_type = getType();
  handleVariableDefinition(var_type, var_segment);

  // Check for more elements in the parameter list. Either we have hit the end
  // of the list (signified by `)`) or we get a `,` signifying more parameters.
  while (!currentTokenIsExpectedSymbol(')')) {
    // We have another parameter.
    if (currentTokenIsExpectedSymbol(',')) {
      // Advance past the comma.
      tokenizer_->nextToken();

      // Get type and name of next variable in the parameter list.
      var_type = getType();
      handleVariableDefinition(var_type, var_segment);
    } else {
      throw ExpectedClosingParenthesis(
        tokenizer_->tokenToString(), ")", parameter_list_tag);
    }
  }
  return;
}

void CompilationEngine::compileStatements() {
  const std::string statements_tag = "statements";

  // Loop through statements, compiling each one.
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
  return;
}

void CompilationEngine::compileReturn() {
  const std::string return_tag = "returnStatement";

  // We know the first word is return as that is the precondition for entering
  // this method. So we just advance past it.
  tokenizer_->nextToken();

  // We haven't hit statement end so we have the form `return expression;`
  if (!currentTokenIsExpectedSymbol(';')) {
    compileExpression();
  } else {
    // In this case we just have a simple `return;` statement. But even void
    // methods need to return something so we push 0 onto the stack, expecting
    // the caller will pop it off.
    vm_writer_->writePush(Segment::CONSTANT, /*idx=*/0);
  }

  // now we expect statement end.
  handleStatementEnd(return_tag);
  vm_writer_->writeReturn();
  return;
}

// Let statements are of 2 forms `let varName = expression;` or
// `let varName[expression] = expression;`.
void CompilationEngine::compileLet() {
  const std::string let_tag = "letStatement";

  // We know the first word is `let` as that is the precondition for entering
  // this method. So just advance past it.
  tokenizer_->nextToken();

  // expect a valid identifier for the variable name.
  expectIdentifier();
  SymbolData var_data = getVarData(/*var_name=*/tokenizer_->tokenToString());
  tokenizer_->nextToken();

  // If we encounter `[` then we know we are in the second case.
  if (currentTokenIsExpectedSymbol('[')) {
    // Push the base address of the array onto the stack, which is the variable.
    vm_writer_->writePush(var_data.segment, /*idx=*/var_data.offset);

    // Compile the expression for the index and push onto the stack.
    handleOpeningParenthesis('[', let_tag)
    compileExpression();
    handleClosingParenthesis(']', let_tag);

    // Add the 2 top values on the stack: the array base address and index
    vm_writer_->writeArithmetic(OpCommand::ADD);

    // Next, we expect an equal sign.
    if (!currentTokenIsExpectedSymbol('=')) {
      throw ExpectedSymbol(tokenizer_->tokenToString(), "=", let_tag);
    }
    tokenizer_->nextToken();

    // Handle the expression on the right of the `=`. The VM commands for the
    // expression will store the result of the expression on the stack.
    compileExpression();

    // Pop the right hand side off the stack and into temp 0
    vm_writer_->writePop(Segment::TEMP, 0);

    // Now the top most value on the stack is the address of the array entry
    // on the left hand side of the equation. So set the THAT segment to this
    // address, push temp 0 back onto the stack and pop it into THAT 0 which
    // is the value of the array entry on the left hand side.
    vm_writer_->writePop(Segment::POINTER, 1);
    vm_writer_->writePush(Segment::TEMP, 0);
    vm_writer_->writePop(Segment::THAT, 0);
  } else {
    // We just have a simple assignment to a variable given by `var_name`,
    // so just compile the expression and pop the result into the variable.

    if (!currentTokenIsExpectedSymbol('=')) {
      throw ExpectedSymbol(tokenizer_->tokenToString(), "=", let_tag);
    }
    tokenizer_->nextToken();

    // Handle the expression on the right of the `=`. The VM commands for the
    // expression will store the result of the expression on the stack.
    compileExpression();

    // Then we pop the result of the expression off the stack and into the
    // variable on the left of the `=`.
    vm_writer_->writePop(var_data.segment, /*idx=*/var_data.offset);
  }
  // Now we expect statement end.
  handleStatementEnd(let_tag);
  return;
}

void CompilationEngine::compileIf() {
  label_count_++;
  std::string endif_label = constructOutputLabel("ENDIF");
  std::string endelse_label = constructOutputLabel("ENDELSE");
  const std::string if_tag = "ifStatement";

  // We know the first word is `if` as this is the precondition for entering
  // this method. So just advance past it.
  tokenizer_->nextToken();

  compileStatementCondition(if_tag);

  // We have compiled the statement condition so it is the top element on the
  // stack. Now we not it, and if the not is true it means the condition is
  // false so we go to the end of the if.
  vm_writer_->writeArithmetic(OpCommand::NOT);
  vm_writer_->writeIfGoTo(endif_label);

  // Now compile the statements inside the if.
  compileScopedStatements(if_tag);

  if (currentTokenIsExpectedKeyword(Keyword::Type::ELSE)) {
    // We have an else clause, so write before the else we write a goto to the
    // end of the else (as part of the if part), so that if we did enter the
    // if, then we skip the else. Then we write the endif label before the else
    // code, signifying that this is where we jump if the if condition is false.
    vm_writer_->writeGoTo(endelse_label);
    vm_writer_->writeLabel(endif_label);

    tokenizer_->nextToken();
    compileScopedStatements("elseStatement");

    // After we compile all the statements of the else, we add the endelse label
    // to signify this is where we jump to if we skip the else statements.
    vm_writer_->writeLabel(endelse_label);
  } else {
    // Otherwise, we just have a simple if statement, so write the endif label
    // as this is the point we jump to if the if condition is false.
    vm_writer_->writeLabel(endif_label);
  }
  return;
}

void CompilationEngine::compileWhile() {
  label_count_++;
  std::string while_label = constructOutputLabel("WHILE_LOOP");
  std::string end_label = constructOutputLabel("ENDLOOP");
  vm_writer_->writeLabel(while_label);
  const std::string while_tag = "whileStatement";

  // We know the first word is `while` as that is the precondition for entering
  // this method. So just advance past it.
  tokenizer_->nextToken();

  compileStatementCondition(while_tag);

  // We have compiled the statement condition so it is the top element on the
  // stack. Now we not it, and if the not is true it means the condition is
  // false so we go to the end of the loop.
  vm_writer_->writeArithmetic(OpCommand::NOT);
  vm_writer_->writeIfGoTo(end_label);

  compileScopedStatements(while_tag);

  // At the end of the while statements, we go back to the top of the loop,
  // unconditionally.
  vm_writer_->writeGoTo(while_label);

  // After the while loop, need to write the end label so we can skip entering
  // the iteration when the condition is false.
  vm_writer_->writeLabel(end_label);
  return;
}

void CompilationEngine::compileDo() {
  const std::string do_tag = "doStatement";

  // We know the first word is `do` as this is the precondition for entering
  // this method. So just advance past it.
  tokenizer_->nextToken();

  // compile the subroutine call. We put the subroutine name into
  // `function_name`, calculate the number of expressions it is called on,
  // and then compile the subroutine call statement to VM code.
  std::stringstream function_name;
  int n_args = compileSubroutineCall(&function_name);
  vm_writer_->writeCall(function_name.str(), n_args);

  // Expect end of statement.
  handleStatementEnd(do_tag);

  // With a do call we don't use the return value of the function but it is
  // expected that it was pushed onto the stack, so we need to pop it off.
  vm_writer_->writePop(Segment::TEMP, /*idx=*/0);

  return;
}

void CompilationEngine::compileTerm() {
  const std::string term_tag = "term";

  if (tokenizer_->getTokenType() == TokenType::SYMBOL) {
    if (IsUnaryOp(tokenizer_->getSymbol())) {
      // Keep track of the unary operator, compile the term, then add the VM
      // command for the unary operator.
      char unary_op = tokenizer_->getSymbol();
      tokenizer_->nextToken();
      compileTerm();
      vm_writer_->writeArithmetic(GetUnaryOpCommand(unary_op));
    } else if (tokenizer_->getSymbol() == '(') {
      handleOpeningParenthesis('(', term_tag);
      compileExpression();
      handleClosingParenthesis(')', term_tag);
    } else {
      throw InvalidTerm(tokenizer_->tokenToString());
    }
  } else if (currentTokenIsSimpleTerm()) {
    if (tokenizer_->getTokenType() == TokenType::INT_CONST) {
      vm_writer_->writePush(Segment::CONSTANT, tokenizer_->getIntVal());
    }
    if (tokenizer_->getTokenType() == TokenType::KEYWORD) {
      compileKeywordConstant();
    }
    tokenizer_->nextToken();
  } else {
    // we must have an identifier. Either a simple variable name, an array
    // element, or a subroutine call.
    expectIdentifier();
    std::string identifier_name = tokenizer_->tokenToString();
    tokenizer_->nextToken();

    if (currentTokenIsExpectedSymbol('[')) {
      // We have an array, so we start by pushing the base address of the array
      // onto the stack. That is, the array variable itself.
      SymbolData arr_data = getVarData(identifier_name);
      vm_writer_->writePush(arr_data.segment, /*idx=*/arr_data.offset);

      handleOpeningParenthesis('[', term_tag);
      // Compile the expression representing the index. This will be pushed onto
      // the stack after the base address of the array.
      compileExpression();
      handleClosingParenthesis(']', term_tag);

      // Add base address and index, which are the top 2 values on the stack.
      vm_writer_->writeArithmetic(OpCommand::ADD);
      // Pop the address into the THAT segment so that THAT 0 points to the
      // corresponding array value and push that onto the stack.
      vm_writer_->writePop(Segment::POINTER, 1);
      vm_writer_->writePush(Segment::THAT, 0);
    } else if (currentTokenIsExpectedSymbol('(')) {
      // We have a subroutine call of the type `methodName(expressionList)`.
      // So we write the subroutine name and then proceed with handling the
      // brackets and expression list.
      handleOpeningParenthesis('(', term_tag);
      int n_locals = compileExpressionList();
      handleClosingParenthesis(')', term_tag);
      vm_writer_->writeCall(identifier_name, n_locals);
    } else if (currentTokenIsExpectedSymbol('.')) {
      // We have a subroutine call of the type
      // `className.methodName(expressionList)`. Note that
      // `methodName(expressionList)` has the form of a subroutine call.
      tokenizer_->nextToken();

      // Add the class name and `.` to the stream, then compile the rest of
      // the subroutine call.
      std::stringstream function_name;
      int n_args = 0;
      handleSymbolDataForSubroutine(&function_name, &n_args, identifier_name);
      n_args += compileSubroutineCall(&function_name);
      vm_writer_->writeCall(function_name.str(), n_args);
    } else {
      // We just have a simple variable.
      SymbolData var_data = getVarData(identifier_name);
      vm_writer_->writePush(var_data.segment, /*idx=*/var_data.offset);
    }
  }
  return;
}

void CompilationEngine::compileExpression() {
  const std::string expression_tag = "expression";

  // We start with a term.
  compileTerm();

  // Then we check for a binary op to tell us there are more terms in the
  // expression.
  while (currentTokenIsBinaryOp()) {
    char binary_op = tokenizer_->getSymbol();
    tokenizer_->nextToken();
    compileTerm();
    if (IsMathOp(binary_op)) {
      vm_writer_->writeCall(GetMathOpFunction(binary_op), 2);
    } else {
      vm_writer_->writeArithmetic(GetSimpleBinaryOpCommand(binary_op));
    }
  }
  return;
}

int CompilationEngine::compileExpressionList() {
  int num_expressions = 0;
  const std::string expression_list_tag = "expressionList";

  // We have an empty parameter list `()`.
  if (currentTokenIsExpectedSymbol(')')) {
    return num_expressions;
  }

  // expect an expression.
  num_expressions++;
  compileExpression();

  // Check for more elements in the expression list. Either we have hit the end
  // of the list (signified by `)`) or we get a `,` signifying more parameters.
  while (!currentTokenIsExpectedSymbol(')')) {
    // we have another expression.
    if (currentTokenIsExpectedSymbol(',')) {
      // Advance past the `,`.
      tokenizer_->nextToken();

      // Expect an expression.
      num_expressions++;
      compileExpression();
    } else {
      throw ExpectedClosingParenthesis(
        tokenizer_->tokenToString(), ")", expression_list_tag);
    }
  }
  return num_expressions;
}

void CompilationEngine::compileSubroutineDec() {
  scope_list_->startSubroutine();

  const std::string subroutine_tag = "subroutineDec";

  Keyword::Type dec_keyword = getSubroutineDecKeyword();
  tokenizer_->nextToken();

  expectFunctionReturnType();
  tokenizer_->nextToken();

  // Expect a valid identifier for the subroutine name.
  expectIdentifier();
  std::string function_name = constructFunctionNameFromCurrToken();
  tokenizer_->nextToken();

  // If the current subroutine is a method we need to add `this` to the
  // subroutine level symbol table as the first argument.
  if (dec_keyword == Keyword::Type::METHOD) {
    scope_list_->define(/*var_name=*/"this",
                        /*var_type=*/curr_class_,
                        /*var_segment=*/Segment::ARGUMENT);
  }

  // Now we expect a parameter list enclosed in `(` and `)`.
  handleOpeningParenthesis('(', subroutine_tag);
  compileParameterList();
  handleClosingParenthesis(')', subroutine_tag);

  // Lastly we compile the body of the subroutine.
  compileSubroutineBody(function_name, dec_keyword);
  return;
}

void CompilationEngine::compileSubroutineBody(
  std::string subroutine_name, Keyword::Type dec_keyword) {
  const std::string subroutine_tag = "subroutineBody";

  handleOpeningParenthesis('{', subroutine_tag);

  while (currentTokenIsExpectedKeyword(Keyword::Type::VAR)) {
    compileVarDec();
    tokenizer_->nextToken();
  }

  // The number of local variables for the function.
  int n_locals = scope_list_->varCount(Segment::LOCAL);
  vm_writer_->writeFunction(subroutine_name, n_locals);

  compileSubroutineInitCode(dec_keyword);

  if (currentTokenIsStatementKeyword()) {
    compileStatements();
  }

  handleClosingParenthesis('}', subroutine_tag);
  return;
}

void CompilationEngine::compileSubroutineInitCode(Keyword::Type dec_keyword) {
  if (dec_keyword == Keyword::Type::CONSTRUCTOR) {
    // Push the number of class attributes onto the stack and allocate the
    // amount of memory needed for those attributes.
    int n_attributes = scope_list_->varCount(Segment::THIS);
    vm_writer_->writePush(Segment::CONSTANT, n_attributes);
    vm_writer_->writeCall("Memory.alloc", 1);

    // After calling `Memory.alloc`, the base address of the allocated memory
    // segment is returned and place at the top of the stack. So we pop this
    // off and place it in the address of pointer 0, referring to THIS.
    vm_writer_->writePop(Segment::POINTER, 0);
  } else if (dec_keyword == Keyword::Type::METHOD) {
    // Push argument 0 (representing `this`) onto the stack and pop it into the
    // THIS segment so that we now anchor it on the correct object.
    vm_writer_->writePush(Segment::ARGUMENT, 0);
    vm_writer_->writePop(Segment::POINTER, 0);
  }
}

void CompilationEngine::setJackFile(std::string jack_file) {
  tokenizer_ = std::make_unique<Tokenizer>(jack_file);
  vm_writer_ = std::make_unique<VMWriter>(jack_file);
  label_count_ = 0;
}

// A subroutine call has one of 2 forms: `subroutineName(expressionList)` or
// `className.subroutineName(expressionList)`.
int CompilationEngine::compileSubroutineCall(
  std::stringstream* function_name) {
  const std::string call_tag = "subroutineCall";
  // The number of arguments to the subroutine.
  int n_args = 0;

  // expect an identifier (either `subroutineName`, `varName`, or `className`).
  expectIdentifier();
  std::string identifier_name = tokenizer_->tokenToString();
  tokenizer_->nextToken();

  // This means we are in the second case. So we handle the identifier
  // representing a variable or class name. Then we handle the subroutine name.
  if (currentTokenIsExpectedSymbol('.')) {
    handleSymbolDataForSubroutine(function_name, &n_args, identifier_name);

    // Advance past the `.`. And assign the subroutine name to the identifier.
    tokenizer_->nextToken();
    identifier_name = tokenizer_->tokenToString();
    tokenizer_->nextToken();
  } else {
    // We have a plain subroutine call. If the stream is currently empty then
    // it is calling a subroutine in the same class. Otherwise, the call arises
    // from a let statement, so the call has already been added. Note that
    // the subroutine implicitly refers to the current object, which is this.
    // (ie. if the call is `subroutine(x, y)` inside the `Obj` class then
    // the call is implicitly `this.subroutine(x, y)` which should be translated
    // to `Obj.subroutine(this, x, y)`).
    if (function_name->rdbuf()->in_avail() == 0) {
      (*function_name) << curr_class_ << '.';
      // Pushing the first argument `this` onto the stack.
      vm_writer_->writePush(Segment::POINTER, 0);
      n_args += 1;
    }
  }

  // Now we add the subroutine name to the stream.
  (*function_name) << identifier_name;

  // Now compile the expression list.
  handleOpeningParenthesis('(', call_tag);
  n_args += compileExpressionList();
  handleClosingParenthesis(')', call_tag);

  return n_args;
}

void CompilationEngine::compileAdditionalVarDecs(
  std::string var_type, Segment var_segment, const std::string compile_tag) {
  while (!currentTokenIsExpectedSymbol(';')) {
    if (currentTokenIsExpectedSymbol(',')) {
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

void CompilationEngine::compileKeywordConstant() {
  switch (tokenizer_->getKeyword()) {
    case Keyword::Type::TRUE:
      vm_writer_->writePush(Segment::CONSTANT, 1);
      vm_writer_->writeArithmetic(OpCommand::NEG);
      return;
    case Keyword::Type::FALSE:
      vm_writer_->writePush(Segment::CONSTANT, 0);
      return;
    case Keyword::Type::THIS:
      vm_writer_->writePush(Segment::POINTER, 0);
    default:
      return;
  }
}

SymbolData CompilationEngine::getVarData(std::string var_name) {
  SymbolData var_data = scope_list_->getVarData(var_name);
  if (var_data.segment == Segment::UNKNOWN) {
    throw UndeclaredVariable(var_name);
  }
  return var_data;
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

Keyword::Type CompilationEngine::getSubroutineDecKeyword() {
  if (tokenizer_->getTokenType() == TokenType::KEYWORD) {
    Keyword::Type keyword = tokenizer_->getKeyword();
    if ((keyword == Keyword::Type::FUNCTION) ||
        (keyword == Keyword::Type::METHOD) ||
        (keyword == Keyword::Type::CONSTRUCTOR)) {
      return keyword;
    }
  }
  throw InvalidSubroutineDecKeyword(tokenizer_->tokenToString());
}

bool CompilationEngine::currentTokenIsBinaryOp() {
  if ((tokenizer_->getTokenType() == TokenType::SYMBOL) &&
      ((IsSimpleBinaryOp(tokenizer_->getSymbol())) ||
       (IsMathOp(tokenizer_->getSymbol())))) {
    return true;
  }
  return false;
}

std::string CompilationEngine::constructFunctionNameFromCurrToken() {
  std::stringstream ss;
  ss << curr_class_ << '.' << tokenizer_->tokenToString();
  return ss.str();
}

void CompilationEngine::handleVariableDefinition(
  std::string var_type, Segment var_segment) {
  std::string var_name = getVarName();
  scope_list_->define(var_name, var_type, var_segment);
}

std::string CompilationEngine::getType() {
  expectType();
  std::string var_type = tokenizer_->tokenToString();
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
  return;
}

void CompilationEngine::handleOpeningParenthesis(
  char parenthesis, const std::string compile_tag) {
  if (!currentTokenIsExpectedSymbol(parenthesis)) {
    throw ExpectedOpeningParenthesis(
      tokenizer_->tokenToString(), std::string(1, parenthesis), compile_tag);
  }
  tokenizer_->nextToken();
  return;
}

void CompilationEngine::handleClosingParenthesis(
  char parenthesis, const std::string compile_tag) {
  if (!currentTokenIsExpectedSymbol(parenthesis)) {
    throw ExpectedClosingParenthesis(
      tokenizer_->tokenToString(), std::string(1, parenthesis), compile_tag);
  }
  tokenizer_->nextToken();
  return;
}

void CompilationEngine::handleSymbolDataForSubroutine(
  std::stringstream* function_name, int* n_args, std::string identifier_name) {
  // Check the symbol table to see if the identifier is the name of a variable.
  SymbolData var_data = scope_list_->getVarData(identifier_name);
  if (var_data.segment == Segment::UNKNOWN) {
    // We have a class name, so stream the class name and the `.`
    (*function_name) << identifier_name << '.';
  } else {
    // We have the form `varName.subroutineName(...)` so the `varName` is
    // the first argument to the subroutine.
    vm_writer_->writePush(var_data.segment, /*idx=*/var_data.offset);
    (*n_args)++;

    // The type of the variable is the class name, so stream this.
    (*function_name) << var_data.symbol_type << '.';
  }
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

std::string CompilationEngine::constructOutputLabel(std::string label) {
  std::stringstream output_label;
  output_label << label << (label_count_ - 1);
  return output_label.str();
}
