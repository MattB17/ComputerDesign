// Normally this would be implemented as a linked list of symbol tables,
// representing successive scopes. The node at the head of the linked list
// represents the narrowest scope, with each successive node representing a
// wider scope.
// However, the Jack language only has 2 scopes: class level and subroutine
// level. So we use only 2 attributes for the 2 scopes, with the subroutine
// scope possibly being null, when compiling at the class level. Note the
// class level is never null because all code must be compiled inside a class.
#ifndef SCOPE_LIST_H
#define SCOPE_LIST_H

#include <memory>
#include <string>

#include "segment.h"
#include "symbol_table.h"

class ScopeList {
public:
  ScopeList();
  ScopeList(const ScopeList&) = delete;
  ScopeList &operator=(const ScopeList&) = delete;
  ScopeList(ScopeList&&) = delete;
  ScopeList &operator=(ScopeList&&) = delete;
  ~ScopeList() {}

private:
  std::unique_ptr<ClassTable> class_scope_;
  std::unique_ptr<SubroutineTable> subroutine_scope_;
};

#endif  // SCOPE_LIST_H
