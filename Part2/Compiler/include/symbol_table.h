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
  SymbolTable() {}
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

// Specialization of a symbol table to handle class variables.
class ClassTable : public SymbolTable {
public:
  ClassTable();
  ClassTable(const ClassTable&) = delete;
  ClassTable &operator=(const ClassTable&) = delete;
  ClassTable(ClassTable&&) = delete;
  ClassTable &operator=(ClassTable&&) = delete;
  virtual ~ClassTable() {}

  void addFieldVar(std::string var_name, std::string var_type);

  void addStaticVar(std::string var_name, std::string var_type);
};

// Specialization of a symbol table to handle subroutine variables.
class SubroutineTable : public SymbolTable {
public:
  SubroutineTable();
  SubroutineTable(const SubroutineTable&) = delete;
  SubroutineTable &operator=(const SubroutineTable&) = delete;
  SubroutineTable(SubroutineTable&&) = delete;
  SubroutineTable &operator=(SubroutineTable&&) = delete;
  ~SubroutineTable() {}

  void addArgumentVar(std::string var_name, std::string var_type);

  void addLocalVar(std::string var_name, std::string var_type);
};

#endif  // SYMBOL_TABLE_H
