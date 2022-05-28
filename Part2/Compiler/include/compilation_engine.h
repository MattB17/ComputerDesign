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
  CompilationEngine() {}
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

  // compiles a list of parameters, adding each to the symbol table as an
  // argument for the subroutine.
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

  // compiles an expression list and returns the number of expressions in the
  // list.
  int compileExpressionList();

  // Compiles a subroutine declaration.
  void compileSubroutineDec();

  // compiles the body of the subroutine `subroutine_name`. `is_constructor`
  // identifies whether the subroutine is a constructor.
  void compileSubroutineBody(std::string subroutine_name, bool is_constructor);

private:
  // Sets the jack file currently being compiled.
  void setJackFile(std::string jack_file);

  // compiles a subroutine call and the name of the subroutine is streamed into
  // `function_name`.
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

  // Compiles a keyword constant to VM code.
  void compileKeywordConstant();

  // Gets the associated `SymbolData` for `var_name` from the scope list. An
  // error is generated if the variable is not in the scope list.
  SymbolData getVarData(std::string var_name);

  // Handles the expectation that the compiler expects to receive keyword `k`.
  // If `k` is not the next token, an exception is thrown.
  void expectKeyword(Keyword::Type k);

  // Handles the expectation of the compiler receiving a valid type. A type is
  // either `int`, `char`, `bool`, or a valid identifier representing a class.
  void expectType();

  // Handles the expection of the compiler receiving a valid identifier.
  void expectIdentifier();

  // Retrieves a subroutine declaration keyword from the current token. That is,
  // one of `constructor`, `function`, `method`. If the current token is not a
  // subroutine declaration keyword, an error is thrown.
  Keyword::Type getSubroutineDecKeyword();

  // Handles the expectation of the compiler receiving a valid function return
  // type. A function return type is either `void`, `int`, `char`, `bool`, or
  // a valid identifier representing a class.
  void expectFunctionReturnType();

  // Retrieves a variable name from the current token and adds the variable
  // definition to the symbol table under segment `var_segment` and with type
  // `var_type`.
  void handleVariableDefinition(std::string var_type, Segment var_segment);

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

  // Streams the class associated with `identifier_name` into `function_name`.
  // If `identifier_name` is the name of a class, the class is streamed into
  // `function_name`. Otherwise, it is the name of a variable, the type of the
  // variable identifies the function to which it belongs and the variable is
  // the first argument to the function.
  void handleSymbolDataForSubroutine(
    std::stringstream* function_name, int* n_args, std::string identifier_name);

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

  // Constructs an output label from `label`, accounting for the `label_count_`.
  std::string constructOutputLabel(std::string label);

  // The tokenizer used to retrieve tokens from the jack file being compiled.
  std::unique_ptr<Tokenizer> tokenizer_;

  // The VM Writer used to write compiled VM code to the appropriate VM file.
  std::unique_ptr<VMWriter> vm_writer_;

  // The name of the current class being compiled.
  std::string curr_class_;

  // Handles the scope hierarchy throughout compilation of a class.
  std::unique_ptr<ScopeList> scope_list_;

  // The count of the number of labels used in the current compilation.
  int label_count_;
};

#endif  // COMPILATION_ENGINE_H
