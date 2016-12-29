#ifndef __COMPILER_SYNTAX_TREE_HPP_
#define __COMPILER_SYNTAX_TREE_HPP_

#include "iostream"
#include "parser.hpp"
#include "symbol.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

using std::shared_ptr;
using std::function;

class parser_ll1;
struct syntax_tree;

struct syntax_tree {
  function<std::string(parser_ll1 *p, syntax_tree *root)> action;
  symbol type;
  vector<shared_ptr<syntax_tree>> children;
  vector<int> attribute_resolves;
  inline syntax_tree *get_child(int index_in_quad) {
    if (attribute_resolves.empty() || index_in_quad >= attribute_resolves.size()) {
      return nullptr;
    }
    auto &index = attribute_resolves[index_in_quad];
    if (children.empty() || index >= children.size()) {
      return nullptr;
    }
    return children[index].get();
  }
  inline void set_resolve(const syntax_rule &resolve) {
    attribute_resolves = resolve.operation;
    action = resolve.action;
  }
  explicit syntax_tree(const symbol type) : action(nullptr), type(type) {}
};

#endif
