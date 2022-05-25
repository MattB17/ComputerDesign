// Used to handle the compilation process of a single Jack file. It accepts
// the Jack file as input, creates a tokenizer, and parses the input, writing
// the compiled output to an XML file.
#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include <memory>
#include <string>

#include "scope_list.h"
#include "tokenizer.h"
#include "vm_writer.h"

class CompilationEngine {
public:
  CompilationEngine();
  CompilationEngine(const CompilationEngine&) = delete;
  CompilationEngine &operator=(const CompilationEngine&) = delete;
  CompilationEngine(CompilationEngine&&) = delete;
  CompilationEngine &operator=(CompilationEngine&&) = delete;
  ~CompilationEngine() {}

  // Runs the compilation process. Translating the jack file to an XML file.
  void compile(std::string jack_file);

  // Runs the compilation process for an entire class.
  void compileClass();

  // Compiles a variable declaration statement.
  void compileVarDec();

  // Compiles a static variable declaration, or a field declaration.
  void compileClassVarDec();

  // compiles a list of parameters, and returns the number of parameters in the
  // list.
  int compileParameterList();

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

  // compiles an expression list and returns the number of expressions in the
  // list.
  int compileExpressionList();

  // Compiles a subroutine declaration.
  void compileSubroutineDec();

  // compiles the body of a subroutine.
  void compileSubroutineBody();

private:
  // Sets the jack file currently being compiled.
  void setJackFile(std::string jack_file);

  // compiles a subroutine call and returns the number of arguments supplied
  // to the subroutine call.
  int compileSubroutineCall(std::stringstream* function_name);

  // Compiles additional variables listed in a variable declaration statement.
  // `var_type` and `var_segment` represent the variable segment and type,
  // respectively, of the variables being compiled. `compile_tag` is a string
  // representing the name of the statement being compiled.
  void compileAdditionalVarDecs(
    std::string var_type, Segment var_segment, const std::string compile_tag);

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

  // Writes `token` to the XML stream with `tag`.
  void writeTagForToken(std::string tag, std::string token);

  // Writes `token` to the XML stream with `tag` and appends the newline
  // character.
  void writeTerminatedTagForToken(std::string tag, std::string token);

  // Gets the associated `SymbolData` for `var_name` from the scope list. An
  // error is generated if the variable is not in the scope list.
  SymbolData getVarData(std::string var_name);

  // Writes `var_name` to the XML stream with the appropriate variable segment
  // tag and appends the newline character. If `expect_definition` is true, then
  // it is expected that `var_name` is in the scope_list. Otherwise,
  // `default_is_class` signifies whether `var_name` should be treated as a
  // class or as a subroutine if not found in the scope list.
  void writeTerminatedVarTag(
    std::string var_name, bool expect_definition, bool default_is_class=true);

  // Writes `var_name` to the XML stream with the appropriate variable segment
  // tag. If `expect_definition` is true, then it is expected that `var_name`
  // is in the scope list. Otherwise, `default_is_class` signifies whether
  // `var_name` should be treated as a class or as a subroutine if not found in
  // the scope list.
  void writeVarTag(
    std::string var_name, bool expect_definition, bool default_is_class=true);

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
  // of `constructor`, `function`, `method`. If the keyword is `method` then
  // `this` is added as the first argument to the subroutine symbol table.
  void expectSubroutineDecKeyword();

  // Handles the expectation of the compiler receiving a valid function return
  // type. A function return type is either `void`, `int`, `char`, `bool`, or
  // a valid identifier representing a class.
  void expectFunctionReturnType();

  // Retrieves a variable name from the current token and adds the variable
  // definition to the symbol table under segment `var_segment` and with type
  // `var_type`.
  void handleVariableDefinition(std::string var_type, Segment var_segment);

  // Handles the outputing of a variable and its tag to the output XML stream.
  void handleVarOutput(std::string var_name, SymbolData var_data);

  // Retrieves a type from the current token and advances the tokenizer. If the
  // current token does not represent a valid type, an error is raised.
  std::string getType();

  // Retrieves a variable name for the current token and advances the tokenizer.
  // If the current token does not represent a valid identifier, an error is
  // raised.
  std::string getVarName();

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

  // Uses the current token to generate the function name for a subroutine. The
  // name is the class name, followed by `.`, followed by the subroutine name.
  std::string constructFunctionNameFromCurrToken();

  // Writes `num_tabs_` tabs to the `xml_stream_`.
  void writeSpaces();

  std::string constructOutputLabel(std::string label);

  // The tokenizer used to retrieve tokens from the jack file being compiled.
  std::unique_ptr<Tokenizer> tokenizer_;

  // The VM Writer used to write compiled VM code to the appropriate VM file.
  std::unique_ptr<VMWriter> vm_writer_;

  // The name of the current class being compiled.
  std::string curr_class_;

  // Handles the scope hierarchy throughout compilation of a class.
  std::unique_ptr<ScopeList> scope_list_;

  // A stream representing the to which the compilation engine writes output.
  std::ofstream xml_stream_;

  // The number of spaces (or level of nesting) for the xml tags.
  int num_spaces_;

  // The count of the number of labels used in the current compilation.
  int label_count_;
};

#endif  // COMPILATION_ENGINE_H
