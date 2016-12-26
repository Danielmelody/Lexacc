#ifndef __COMPILER_SYNTAX_TREE_HPP_
#define __COMPILER_SYNTAX_TREE_HPP_

#include "iostream"
#include "parser.hpp"
#include "symbol.hpp"
#include <memory>
#include <vector>

using std::shared_ptr;

struct syntax_tree {
  const symbol type;
  syntax_tree *lhs;
  syntax_tree *rhs;
  explicit syntax_tree(const symbol &type) : type(type) {}
};

#endif
