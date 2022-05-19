// A class to store the variables in a particular scope.
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

#include "segment.h"

struct SymbolData {
  std::string symbol_type;
  Segment segment;
  int offset;
};

class SymbolTable {
public:
  SymbolTable();
  SymbolTable(const SymbolTable&) = delete;
  SymbolTable &operator=(const SymbolTable&) = delete;
  SymbolTable(SymbolTable&&) = delete;
  SymbolTable &operator=(SymbolTable&&) = delete;
  ~SymbolTable() {}

  int getSegmentCount(Segment segment);

  Segment getVarKind(std::string var_name);

  std::string getVarType(std::string var_name);

  int getVarIndex(std::string var_name);
private:
  void addSymbol(
    std::string symbol_name, std::string symbol_type, Segment symbol_segment);

  std::unordered_map<std::string, SymbolData> symbol_map_;
  std::unordered_map<Segment, int> segment_counts_;
};

#endif  // SYMBOL_TABLE_H
