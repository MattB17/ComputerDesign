// Used to handle the compilation process of a single Jack file. It accepts
// the Jack file as input, creates a tokenizer, and parses the input, writing
// the compiled output to an XML file.
#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include <memory>
#include <string>

#include "tokenizer.h"

class CompilationEngine {
public:
  CompilationEngine(std::string jack_file);
  CompilationEngine(const CompilationEngine&) = delete;
  CompilationEngine &operator=(const CompilationEngine&) = delete;
  CompilationEngine(CompilationEngine&&) = delete;
  CompilationEngine &operator=(CompilationEngine&&) = delete;
  ~CompilationEngine() {}

  // Runs the compilation process. Translating the jack file to an XML file.
  void compile();

  // Runs the compilation process for an entire class.
  void compileClass();

  // Compiles a variable declaration statement.
  void compileVarDec();

  // Compiles a static variable declaration, or a field declaration.
  void compileClassVarDec();

  // compiles a list of parameters.
  void compileParameterList();

  // compiles a collection of statements.
  void compileStatements();

  // compiles a do statement.
  void compileDo();

  // compiles an if statement.
  void compileIf();

  // compiles a while statement.
  void compileWhile();

  // Compiles a let statement.
  void compileLet();

  // compiles a return statement.
  void compileReturn();

  // compiles a term.
  void compileTerm();

  // compiles an expression
  void compileExpression();

  // compiles an expression list.
  void compileExpressionList();

  // Compiles a subroutine declaration.
  void compileSubroutineDec();

  // compiles the body of a subroutine.
  void compileSubroutineBody();

private:
  // compiles a subroutine call.
  void compileSubroutineCall();

  // Compiles additional variables listed in a variable declaration statement.
  // `compile_tag` is a string representing the name of the statement being
  // compiled.
  void compileAdditionalVarDecs(const std::string compile_tag);

  // Compiles the condition of an if or while statement. The condition has the
  // form `(expression)` as in `if (expression) { ... }`.
  void compileStatementCondition(const std::string compile_tag);

  // Compiles a scoped set of statements. That is, a set of statements between
  // `{` and `}`.
  void compileScopedStatements(const std::string compile_tag);

  // Writes the current token to the XML stream with the appropriate tag.
  void writeTokenWithTag();

  // Writes the XML opening tag for `tag`.
  void writeOpenTag(const std::string tag);

  // Writes the XML closing tag for `tag`.
  void writeCloseTag(const std::string tag);

  // Writes the current token to the XML stream with the appropriate tag and
  // appends the newline character.
  void writeTerminatedTokenAndTag();

  // Writes the XML opening tag for `tag` and appends the newline character.
  void writeTerminatedOpenTag(const std::string tag);

  // Writes the XML closing tag for `tag` and appends the newline character.
  void writeTerminatedCloseTag(const std::string tag);

  // Handles the expectation that the compiler expects to receive keyword `k`.
  // If `k` is not the next token, an exception is thrown.
  void expectKeyword(Keyword::Type k);

  // Handles the expectation of the compiler receiving a valid type. A type is
  // either `int`, `char`, `bool`, or a valid identifier representing a class.
  void expectType();

  // Handles the expection of the compiler receiving a valid identifier.
  void expectIdentifier();

  // Handles the expectation of a subroutine declaration keyword. That is, one
  // of `constructor`, `function`, `method`.
  void expectSubroutineDecKeyword();

  // Handles the expectation of the compiler receiving a valid function return
  // type. A function return type is either `void`, `int`, `char`, `bool`, or
  // a valid identifier representing a class.
  void expectFunctionReturnType();

  // Handles a type followed by an identifier. An exception is thrown if the
  // next 2 tokens are not a type and an identifier respectively.
  void handleTypeAndIdentifierPair();

  // Handles the end of a statement. That is, expects and outputs the token `;`.
  // If the token is not found, an error is generated.
  void handleStatementEnd(const std::string compile_tag);

  // Handles the expectation of an opening parenthesis `parenthesis`. If the
  // parenthesis is not found, an error is generated.
  void handleOpeningParenthesis(
    char parenthesis, const std::string compile_tag);

  // Handles the expectation of a closing parenthesis `parenthesis`. If the
  // parenthesis is not found, an error is generated.
  void handleClosingParenthesis(
    char parenthesis, const std::string compile_tag);

  // Returns whether the current token pointed to by the tokenizer is the
  // symbol `expected_symbol`.
  bool currentTokenIsExpectedSymbol(char expected_symbol);

  // Returns whether the current token pointed to by the tokenizer is the
  // keyword `k`.
  bool currentTokenIsExpectedKeyword(Keyword::Type k);

  // Returns whether the current token pointed to by the tokenizer is a
  // statement keyword, one of: `let`, `if`, `while`, `do`, `return`.
  bool currentTokenIsStatementKeyword();

  // Returns whether the current token pointed to by the tokenizer is a
  // class var keyword, one of: `static` or `field`.
  bool currentTokenIsClassVarKeyword();

  // Returns whether the current token pointed to by the tokenizer is a simple
  // term. That is, an integer constant, a string constant, or a keyword
  // constant - one of: `true`, `false`, `null`, or `this`.
  bool currentTokenIsSimpleTerm();

  // Returns whether the current token pointed to by the tokenizer is a binary
  // operand.
  bool currentTokenIsBinaryOp();

  // Writes `num_tabs_` tabs to the `xml_stream_`.
  void writeTabs();

  // The tokenizer used to retrieve tokens from the jack file being compiled.
  std::unique_ptr<Tokenizer> tokenizer_;

  // A stream representing the to which the compilation engine writes output.
  std::ofstream xml_stream_;

  // The number of tabs (or level of nesting) for the xml tags.
  int num_tabs_;
};

#endif  // COMPILATION_ENGINE_H
