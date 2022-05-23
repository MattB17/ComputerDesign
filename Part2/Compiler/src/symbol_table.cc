#include "exceptions.h"
#include "symbol_table.h"

int SymbolTable::getSegmentCount(Segment segment) {
  auto itr = segment_counts_.find(segment);
  if (itr == segment_counts_.end()) {
    return 0;
  }
  return itr->second;
}

SymbolData SymbolTable::getSymbolData(std::string var_name) {
  auto itr = symbol_map_.find(var_name);
  if (itr == symbol_map_.end()) {
    return {"", Segment::UNKNOWN, -1};
  }
  return itr->second;
}

void SymbolTable::addSymbol(
  std::string symbol_name, std::string symbol_type, Segment symbol_segment) {
  // check for redefinition of symbol.
  if (symbol_map_.find(symbol_name) != symbol_map_.end()) {
    throw RedefinitionOfSymbol(symbol_name);
  }

  // otherwise, add it to the symbol table.
  symbol_map_[symbol_name] = {
    symbol_type, symbol_segment, segment_counts_[symbol_segment]};
  segment_counts_[symbol_segment]++;
}

ClassTable::ClassTable() : SymbolTable() {
  segment_counts_ = {{Segment::THIS, 0},
                     {Segment::STATIC, 0}};
}

void ClassTable::addFieldVar(std::string var_name, std::string var_type) {
  addSymbol(var_name, var_type, Segment::THIS);
}

void ClassTable::addStaticVar(std::string var_name, std::string var_type) {
  addSymbol(var_name, var_type, Segment::STATIC);
}

SubroutineTable::SubroutineTable() : SymbolTable() {
  segment_counts_ = {{Segment::ARGUMENT, 0},
                     {Segment::LOCAL, 0}};
}

void SubroutineTable::addArgumentVar(
  std::string var_name, std::string var_type) {
  addSymbol(var_name, var_type, Segment::ARGUMENT);
}

void SubroutineTable::addLocalVar(std::string var_name, std::string var_type) {
  addSymbol(var_name, var_type, Segment::LOCAL);
}
