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

int CompilationEngine::compileParameterList() {
  int num_params = 0;
  Segment var_segment = Segment::ARGUMENT;

  const std::string parameter_list_tag = "parameterList";
  writeTerminatedOpenTag(parameter_list_tag);

  // We have an empty parameter list `()`.
  if (currentTokenIsExpectedSymbol(')')) {
    writeTerminatedCloseTag(parameter_list_tag);
    return num_params;
  }

  // Get variable type and name as the parameter list is not empty.
  num_params++;
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
      num_params++;
      var_type = getType();
      handleVariableDefinition(var_type, var_segment);
    } else {
      throw ExpectedClosingParenthesis(
        tokenizer_->tokenToString(), ")", parameter_list_tag);
    }
  }
  writeTerminatedCloseTag(parameter_list_tag);
  return num_params;
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
  } else {
    // In this case we just have a simple `return;` statement. But even void
    // methods need to return something so we push 0 onto the stack, expecting
    // the caller will pop it off.
    vm_writer_->writePush(Segment::CONSTANT, /*idx=*/0);
  }

  // now we expect statement end.
  handleStatementEnd(return_tag);

  writeTerminatedCloseTag(return_tag);
  vm_writer_->writeReturn();
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
  std::string var_name = tokenizer_->tokenToString();
  writeTerminatedVarTag(var_name, /*expect_definition=*/true);
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

  // handle the expression on the right of the `=`. The VM commands for the
  // expression will store the result of the expression on the stack.
  compileExpression();

  // Then we pop the result of the expression off the stack and into the
  // variable on the left of the `=`.
  SymbolData var_data = getVarData(var_name);
  vm_writer_->writePop(var_data.segment, /*idx=*/var_data.offset);

  // now we expect statement end.
  handleStatementEnd(let_tag);

  writeTerminatedCloseTag(let_tag);
  return;
}

void CompilationEngine::compileIf() {
  std::string endif_label = constructOutputLabel("ENDIF");
  const std::string if_tag = "ifStatement";
  writeTerminatedOpenTag(if_tag);

  // write keyword if.
  writeTerminatedTokenAndTag();

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
    std::string endelse_label = constructOutputLabel("ENDELSE");
    // We have an else clause, so write before the else we write a goto to the
    // end of the else (as part of the if part), so that if we did enter the
    // if, then we skip the else. Then we write the endif label before the else
    // code, signifying that this is where we jump if the if condition is false.
    vm_writer_->writeGoTo(endelse_label);
    vm_writer_->writeLabel(endif_label);
    writeTerminatedTokenAndTag();

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

  writeTerminatedCloseTag(if_tag);
  label_count_++;
  return;
}

void CompilationEngine::compileWhile() {
  std::string while_label = constructOutputLabel("WHILE_LOOP");
  std::string end_label = constructOutputLabel("ENDLOOP");
  vm_writer_->writeLabel(while_label);
  const std::string while_tag = "whileStatement";
  writeTerminatedOpenTag(while_tag);

  // write keyword while.
  writeTerminatedTokenAndTag();

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

  writeTerminatedCloseTag(while_tag);

  // After the while loop, need to write the end label so we can skip entering
  // the iteration when the condition is false.
  vm_writer_->writeLabel(end_label);
  label_count_++;
  return;
}

void CompilationEngine::compileDo() {
  const std::string do_tag = "doStatement";
  writeTerminatedOpenTag(do_tag);

  // write keyword do.
  writeTerminatedTokenAndTag();
  tokenizer_->nextToken();

  // compile the subroutine call. We put the subroutine name into
  // `function_name`, calculate the number of expressions it is called on,
  // and then compile the subroutine call statement to VM code.
  std::stringstream function_name;
  int n_locals = compileSubroutineCall(&function_name);
  vm_writer_->writeCall(function_name.str(), n_locals);

  // Expect end of statement.
  handleStatementEnd(do_tag);

  // With a do call we don't use the return value of the function but it is
  // expected that it was pushed onto the stack, so we need to pop it off.
  vm_writer_->writePop(Segment::TEMP, /*idx=*/0);

  writeTerminatedCloseTag(do_tag);
  return;
}

void CompilationEngine::compileTerm() {
  const std::string term_tag = "term";
  writeTerminatedOpenTag(term_tag);

  if (tokenizer_->getTokenType() == TokenType::SYMBOL) {
    if (IsUnaryOp(tokenizer_->getSymbol())) {
      char unary_op = tokenizer_->getSymbol();
      writeTerminatedTokenAndTag();
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
      int n_locals = compileExpressionList();
      handleClosingParenthesis(')', term_tag);
      vm_writer_->writeCall(identifier_name, n_locals);
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

      // Add the class name and `.` to the stream, then compile the rest of
      // the subroutine call.
      std::stringstream function_name;
      function_name << identifier_name << '.';
      int n_locals = compileSubroutineCall(&function_name);
      vm_writer_->writeCall(function_name.str(), n_locals);
    } else {
      // it's just a simple identifier for a variable name.
      writeTerminatedVarTag(
        /*var_name=*/identifier_name, /*expect_definition=*/true);
      SymbolData var_data = getVarData(identifier_name);
      vm_writer_->writePush(var_data.segment, /*idx=*/var_data.offset);
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
    char binary_op = tokenizer_->getSymbol();
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();
    compileTerm();
    if (IsMathOp(binary_op)) {
      vm_writer_->writeCall(GetMathOpFunction(binary_op), 2);
    } else {
      vm_writer_->writeArithmetic(GetSimpleBinaryOpCommand(binary_op));
    }
  }

  writeTerminatedCloseTag(expression_tag);
  return;
}

int CompilationEngine::compileExpressionList() {
  int num_expressions = 0;
  const std::string expression_list_tag = "expressionList";
  writeTerminatedOpenTag(expression_list_tag);

  // We have an empty parameter list `()`.
  if (currentTokenIsExpectedSymbol(')')) {
    writeTerminatedCloseTag(expression_list_tag);
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
      writeTerminatedTokenAndTag();
      tokenizer_->nextToken();

      // Expect an expression.
      num_expressions++;
      compileExpression();
    } else {
      throw ExpectedClosingParenthesis(
        tokenizer_->tokenToString(), ")", expression_list_tag);
    }
  }
  writeTerminatedCloseTag(expression_list_tag);
  return num_expressions;
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
  std::string function_name = constructFunctionNameFromCurrToken();
  writeTerminatedTagForToken(
    /*tag=*/"subroutineName", /*token=*/function_name);
  tokenizer_->nextToken();

  // Now we expect a parameter list enclosed in `(` and `)`.
  handleOpeningParenthesis('(', subroutine_tag);
  int n_args = compileParameterList();
  handleClosingParenthesis(')', subroutine_tag);

  vm_writer_->writeFunction(function_name, n_args);

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
  vm_writer_ = std::make_unique<VMWriter>(jack_file);
  label_count_ = 0;
  num_spaces_ = 0;
  xml_stream_.open(jackFileToOutputFile(jack_file, ".xml"));
}

// A subroutine call has one of 2 forms: `subroutineName(expressionList)` or
// `className.subroutineName(expressionList)`.
int CompilationEngine::compileSubroutineCall(
  std::stringstream* function_name) {
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

    (*function_name) << identifier_name << '.';

    // Then write the `.`.
    writeTerminatedTokenAndTag();
    tokenizer_->nextToken();

    // Then recurse.
    return compileSubroutineCall(function_name);
  }

  // Otherwise, we have a plain subroutine call. So we write the subroutine name
  // and then compile the expression list.
  writeTerminatedVarTag(/*var_name=*/identifier_name,
                        /*expect_definition=*/false,
                        /*default_is_class=*/false);

  (*function_name) << identifier_name;

  // Now we expect the start of the expression list.
  handleOpeningParenthesis('(', call_tag);
  int n_locals = compileExpressionList();
  handleClosingParenthesis(')', call_tag);

  return n_locals;
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

void CompilationEngine::compileKeywordConstant() {
  switch (tokenizer_->getKeyword()) {
    case Keyword::Type::TRUE:
      vm_writer_->writePush(Segment::CONSTANT, 1);
      vm_writer_->writeArithmetic(OpCommand::NEG);
      return;
    case Keyword::Type::FALSE:
      vm_writer_->writePush(Segment::CONSTANT, 0);
      return;
    default:
      return;
  }
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
        if (var_data.segment != Segment::UNKNOWN) {
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

SymbolData CompilationEngine::getVarData(std::string var_name) {
  SymbolData var_data = scope_list_->getVarData(var_name);
  if (var_data.segment == Segment::UNKNOWN) {
    throw UndeclaredVariable(var_name);
  }
  return var_data;
}

void CompilationEngine::writeVarTag(
  std::string var_name, bool expect_definition, bool default_is_class) {
  SymbolData var_data = scope_list_->getVarData(var_name);
  if (var_data.segment == Segment::UNKNOWN) {
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

std::string CompilationEngine::constructOutputLabel(std::string label) {
  std::stringstream output_label;
  output_label << label << label_count_;
  return output_label.str();
}
