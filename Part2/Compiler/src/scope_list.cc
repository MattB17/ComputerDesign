#include "exception.h"
#include "scope_list.h"

ScopeNode::ScopeNode()
  : symbol_table(std::make_unique<SymbolTable>()), parent_scope_(nullptr) {}

ScopeList::ScopeList() : curr_scope_(nullptr) {}

ScopeList::~ScopeList() {
  ScopeNode* curr_node = head;
  while (curr_node != nullptr) {
    head = head->getParentScope();
    delete curr_node;
    curr_node = head;
  }
}

void ScopeList::newScope() {
  ScopeNode* new_node = new ScopeNode();
  new_node->setParentScope(head);
  head = new_node;
}

void ScopeList::removeCurrentScope() {
  ScopeNode* curr_scope = head;
  head = head->getParentScope();
  delete curr_scope;
}

void ScopeList::addVariable(std::string var_name, std::string var_type,
                            Segment var_segment, int var_offset) {
  if (head == nullptr) {
    throw InvalidDeclaration(var_name);
  }
  head->getSymbolTable()->addSymbol(
    var_name, var_type, var_segment, var_offset);
}

SymbolData ScopeList::getVariableData(std::string var_name) {
  ScopeNode* curr_scope = head;
  while (curr_scope != nullptr) {
    if (curr_scope->getSymbolTable()->hasSymbol(var_name)) {
      return curr_scope->getSymbolTable()->getSymbolData(var_name);
    }
    curr_scope = curr_scope->getParentScope();
  }
  throw UndefinedSymbol(var_name);
}
