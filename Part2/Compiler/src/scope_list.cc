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
