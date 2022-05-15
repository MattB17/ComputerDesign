// A class to store the variables in a particular scope.
#include <string>
#include <unordered_map>

#include "segment.h"

struct SymbolData {
  std::string type;
  Segment segment;
  int offset;
};

class SymbolTable {
public:
  SymbolTable() {}
  SymbolTable(const SymbolTable&) = delete;
  SymbolTable &operator=(const SymbolTable&) = delete;
  SymbolTable(SymbolTable&&) = delete;
  SymbolTable &operator=(SymbolTable&&) = delete;
  ~SymbolTable() {}

  void addSymbol(std::string symbol_name, std::string symbol_type,
                 Segment symbol_segment, int symbol_offset);

  bool hasSymbol(std::string symbol_name);

  SymbolData getSymbolData(std::string symbol_name);
private:
  std::unordered_map<std::string, SymbolData> symbol_map_;
};
