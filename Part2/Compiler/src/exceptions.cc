#include "exceptions.h"

UndeclaredVariable::UndeclaredVariable(std::string var_name)
  : std::runtime_error(var_name + " used before declaration. Must declare "
                       "variable before it is used.")
{}

InvalidArgumentVarDeclaration::InvalidArgumentVarDeclaration(
  std::string var_name) : std::runtime_error(
    var_name + " declared as an argument variable at class level scope.")
{}

InvalidLocalVarDeclaration::InvalidLocalVarDeclaration(std::string var_name)
  : std::runtime_error(var_name + " declared as a local variable at a class "
                       "level scope.")
{}

InvalidFieldVarDeclaration::InvalidFieldVarDeclaration(std::string var_name)
  : std::runtime_error(
    "Class field " + var_name + " declared inside a subroutine. Class fields "
    "must be defined at the class scope.")
{}

InvalidStaticVarDeclaration::InvalidStaticVarDeclaration(std::string var_name)
  : std::runtime_error(
    "Static variable " + var_name + " declared inside a subroutine. Static "
    "variables must be defined at the class scope.")
{}

InvalidDeclarationStatement::InvalidDeclarationStatement(std::string var_name)
  : std::runtime_error(
    "Invalid declaration for " + var_name + ". Variables must either be "
    "declared as subroutine arguments or in statements starting with one of "
    "the keywords: `var`, `field`, or `static`.")
{}

RedefinitionOfSymbol::RedefinitionOfSymbol(std::string symbol_name)
  : std::runtime_error("Redefinition of variable " + symbol_name + ". " +
                       symbol_name + " already exists in the current scope.")
{}

ExpectedSymbol::ExpectedSymbol(std::string received_token,
                               std::string expected_symbol,
                               std::string compile_tag)
  : std::runtime_error("Expected to receive " + expected_symbol +
                       " as part of " + compile_tag + ". Instead received " +
                       received_token + ".")
{}

ExpectedOpeningParenthesis::ExpectedOpeningParenthesis(
  std::string received_token, std::string expected_parenthesis,
  std::string compile_tag)
  : ExpectedSymbol(received_token, expected_parenthesis, compile_tag)
{}

ExpectedClosingParenthesis::ExpectedClosingParenthesis(
  std::string received_token, std::string expected_parenthesis,
  std::string compile_tag)
  : ExpectedSymbol(received_token, expected_parenthesis, compile_tag)
{}

ExpectedStatementEnd::ExpectedStatementEnd(std::string received_token,
                                           std::string compile_tag)
  : ExpectedSymbol(received_token, ";", compile_tag)
{}

InvalidTerm::InvalidTerm(std::string received_token) : std::runtime_error(
  "A valid term is one of an integer constant, a string constant, a valid "
  "identifier, or a keyword constant: `true`, `false`, `null`, or `this`. "
  "Instead received " + received_token + ".")
{}

InvalidSubroutineDecKeyword::InvalidSubroutineDecKeyword(
  std::string received_token) : std::runtime_error(
  "Expected subroutine declaration keyword, one of: `constructor`, `function`, "
  "or `method`. Instead received " + received_token + ".")
{}

MissingIdentifier::MissingIdentifier(std::string received_token)
  : std::runtime_error("Expected a valid identifier. Instead received " +
                       received_token + ".")
{}

InvalidType::InvalidType(std::string received_token) : std::runtime_error(
  "Expected a valid type. Instead received " + received_token + ".")
{}

InvalidFunctionReturnType::InvalidFunctionReturnType(std::string received_token)
  : std::runtime_error("Expected a valid function return type: either an "
                       "identifier representing a class name or one of "
                       "`void`, `int`, `char`, or `bool`. Instead received " +
                       received_token + ".")
{}

KeywordNotFound::KeywordNotFound(Keyword::Type expected_keyword,
                                 std::string received_token)
  : std::runtime_error("Expected to receive keyword " +
                       Keyword::KeywordToString(expected_keyword) +
                       ", but instead got " + received_token + ".")
{}

InvalidToken::InvalidToken(std::string start_char)
  : std::runtime_error("Tokens must be a symbol, a string, an integer, or a "
                       "valid identifier that does not start with a number. "
                       "Instead received a token starting with " + start_char +
                       ".")
{}

InvalidIdentifier::InvalidIdentifier(std::string start_char)
  : std::runtime_error("Identifiers must contain only '_' or alphanumerics, "
                       "and must not start with a number. Instead, received "
                       "an identifier starting with " + start_char + ".")
{}
