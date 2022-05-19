#include "exceptions.h"
#include "symbol_table.h"

SymbolTable::SymbolTable() {
  segment_counts_ = {{Segment::FIELD, 0},
                     {Segment::STATIC, 0},
                     {Segment::LOCAL, 0},
                     {Segment::ARGUMENT, 0}};
}

int SymbolTable::getSegmentCount(Segment segment) {
  auto itr = segment_counts_.find(segment);
  if (itr == segment_counts_.end()) {
    return 0;
  }
  return itr->second;
}

Segment SymbolTable::getVarKind(std::string var_name) {
  auto itr = symbol_map_.find(var_name);
  if (itr == symbol_map_.end()) {
    return Segment::NONE;
  }
  return itr->second.segment;
}

std::string SymbolTable::getVarType(std::string var_name) {
  auto itr = symbol_map_.find(var_name);
  if (itr == symbol_map_.end()) {
    return "";
  }
  return itr->second.symbol_type;
}

int SymbolTable::getVarIndex(std::string var_name) {
  auto itr = symbol_map_.find(var_name);
  if (itr == symbol_map_.end()) {
    return -1;
  }
  return itr->second.offset;
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
