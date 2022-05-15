// Implements a linked list of symbol tables, representing successive scopes.
// The node at the head of the linked list represents the narrowest scope, with
// each successive node representing a wider scope.
// The Jack language only has 2 scopes: class level and subroutine level.
// However, this could be used for another language with an arbitrary number
// of scopes.
#include <memory>

class ScopeNode {
public:
  ScopeNode();
  ScopeNode(const ScopeNode&) = delete;
  ScopeNode &operator=(const ScopeNode&) = delete;
  ScopeNode(ScopeNode&&) = delete;
  ScopeNode &operator=(ScopeNode&&) = delete;
  ~ScopeNode() {}

  std::unique_ptr<SymbolTable> getSymbolTable() { return symbol_table_; }
  ScopeNode* getParentScope() { return parent_scope_; }
  void setParentScope(ScopeNode* parent_scope) { parent_scope_ = parent_scope; }
private:
  std::unique_ptr<SymbolTable> symbol_table_;
  ScopeNode* parent_scope_;
};

class ScopeList {
public:
  ScopeList();
  ScopeList(const ScopeList&) = delete;
  ScopeList &operator=(const ScopeList&) = delete;
  ScopeList(ScopeList&&) = delete;
  ScopeList &operator=(ScopeList&&) = delete;
  ~ScopeList();

  void newScope();

  void removeCurrentScope();
private:
  ScopeNode* curr_scope_;
}
