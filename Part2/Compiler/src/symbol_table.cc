#include "symbol_table.h"

void SymbolTable::addSymbol(std::string symbol_name, std::string symbol_type,
                            Segment symbol_segment, int symbol_offset) {
  // check for redefinition of symbol.
  if (symbol_map_.find(symbol_name) != symbol_map_.end()) {
    throw RedefinitionOfSymbol(symbol_name);
  }

  // otherwise, add it to the symbol table.
  symbol_map_[symbol_name] = {symbol_type, symbol_segment, symbol_offset};
}

bool SymbolTable::hasSymbol(std::string symbol_name) {
  return (symbol_map_.find(symbol_name) != symbol_map_.end());
}

SymbolData SymbolTable::getSymbolData(std::string symbol_name) {
  auto itr = symbol_map_.find(symbol_name);

  // can't get data if it is not defined.
  if (itr == symbol_map_.end()) {
    throw UndefinedSymbol(symbol_name);
  }

  return itr->second;
}
