#include "scope_list.h"

#include <memory>

#include "exceptions.h"

ScopeList::ScopeList()
  : class_scope_(std::make_unique<ClassTable>()), subroutine_scope_(nullptr)
{}
