#include "scope_list.h"

#include <memory>

#include "exceptions.h"

ScopeList::ScopeList()
  : class_scope_(std::make_unique<ClassTable>()), subroutine_scope_(nullptr)
{}

void ScopeList::startSubroutine() {
  subroutine_scope_ = std::make_unique<SubroutineTable>();
}

void ScopeList::define(
  std::string var_name, std::string var_type, Segment var_segment) {
  switch (var_segment) {
    case Segment::ARGUMENT:
      // if it is null, we are at the class level.
      if (subroutine_scope_ == nullptr) {
        throw InvalidArgumentVarDeclaration(var_name);
      }
      subroutine_scope_->addArgumentVar(var_name, var_type);
      break;
    case Segment::LOCAL:
      // if it is null, we are at the class level.
      if (subroutine_scope_ == nullptr) {
        throw InvalidLocalVarDeclaration(var_name);
      }
      subroutine_scope_->addLocalVar(var_name, var_type);
      break;
    case Segment::THIS:
      // if it is not null, we are inside a subroutine.
      if (subroutine_scope_ != nullptr) {
        throw InvalidFieldVarDeclaration(var_name);
      }
      class_scope_->addFieldVar(var_name, var_type);
      break;
    case Segment::STATIC:
      // if it is not null, we are inside a subroutine.
      if (subroutine_scope_ != nullptr) {
        throw InvalidStaticVarDeclaration(var_name);
      }
      class_scope_->addStaticVar(var_name, var_type);
      break;
    default:
      throw InvalidDeclarationStatement(var_name);
  }
}

int ScopeList::varCount(Segment segment) {
  if ((segment == Segment::ARGUMENT) || (segment == Segment::LOCAL)) {
    if (subroutine_scope_ == nullptr) {
      throw InvalidScope();
    }
    return subroutine_scope_->getSegmentCount(segment);
  }
  if ((segment == Segment::THIS) || (segment == Segment::STATIC)) {
    return class_scope_->getSegmentCount(segment);
  }
  throw InvalidSegmentType();
}

SymbolData ScopeList::getVarData(std::string var_name) {
  if (subroutine_scope_ != nullptr) {
    SymbolData var_data = subroutine_scope_->getSymbolData(var_name);
    if (var_data.segment != Segment::UNKNOWN) {
      return var_data;
    }
  }
  return class_scope_->getSymbolData(var_name);
}
