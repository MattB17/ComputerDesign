#include "exceptions.h"
#include "scope_list.h"

ScopeNode::ScopeNode()
  : symbol_table_(new SymbolTable()), parent_scope_(nullptr) {}

ScopeNode::~ScopeNode() {
  delete symbol_table_;
}

ScopeList::ScopeList() : curr_scope_(nullptr) {}

ScopeList::~ScopeList() {
  ScopeNode* curr_node = curr_scope_;
  while (curr_node != nullptr) {
    curr_scope_ = curr_scope_->getParentScope();
    delete curr_node;
    curr_node = curr_scope_;
  }
}

void ScopeList::newScope() {
  ScopeNode* new_node = new ScopeNode();
  new_node->setParentScope(curr_scope_);
  curr_scope_ = new_node;
}

void ScopeList::removeCurrentScope() {
  ScopeNode* curr_scope = curr_scope_;
  curr_scope_ = curr_scope_->getParentScope();
  delete curr_scope;
}

void ScopeList::addVariable(
  std::string var_name, std::string var_type, Segment var_segment) {
  if (curr_scope_ == nullptr) {
    throw InvalidDeclaration(var_name);
  }
  curr_scope_->getSymbolTable()->addSymbol(var_name, var_type, var_segment);
}

SymbolData ScopeList::getVariableData(std::string var_name) {
  ScopeNode* curr_scope = curr_scope_;
  while (curr_scope != nullptr) {
    if (curr_scope->getSymbolTable()->hasSymbol(var_name)) {
      return curr_scope->getSymbolTable()->getSymbolData(var_name);
    }
    curr_scope = curr_scope->getParentScope();
  }
  throw UndefinedSymbol(var_name);
}
