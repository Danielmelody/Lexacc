#include "actions.hpp"
#include "parser.hpp"
#include <iostream>
#include <string>

using std::cout;
using std::cerr;
using std::endl;

void parser_ll1::emit(string operation, string arg1, string arg2,
                      string result) {
  quads.emplace_back(quad(operation, arg1, arg2, result));
}

string parser_ll1::code_generate(syntax_tree *root) {
  if (root == nullptr) {
    return "";
  }

  for (auto iter = root->children.begin(); iter != root->children.end();) {
    auto child = *iter;
    if (combine_symbols.find(child->type) != combine_symbols.end()) {
      iter = root->children.erase(iter);
      for (auto combine_child : child->children) {
        iter = root->children.insert(iter, combine_child);
        iter++;
      }
      continue;
    }
    ++iter;
  }

  if (root->action) {
    return root->action(this, root);
  } else {
    if (root->children.size() == 1) {
      return actions::ir_next_0(this, root);
    }
    auto lhs = code_generate(root->get_child(0));
    auto rhs = code_generate(root->get_child(1));

    auto result = ctr.temp();
    if (root->type.terminal) {
      return root->type._token.content;
    }
    auto op = root->get_child(2);
    if (op) {
      emit(op->type._token.content, lhs, rhs, result);
    } else {
      auto op = root->type._token.content;
      if (op != "") {
        emit(op, lhs, rhs, result);
      }
    }
    return result;
  }
}

void parser_ll1::print_ir() {
  if (state) {
    return;
  }
  for (int i = 0; i < quads.size(); ++i) {
    cout << i << " " << quads[i].op << ", " << quads[i].arg1 << ", "
         << quads[i].arg2 << ", " << quads[i].result << endl;
  }
}
