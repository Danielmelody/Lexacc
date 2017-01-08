#include "parser.hpp"
#include "quad.hpp"
#include "utilty.hpp"
#include <string>
#include <vector>

using std::vector;
using std::string;

namespace actions {

inline bool is_initialize(string sym, const context_analyzer &ctr) {
  if (ctr.defined(sym)) {
    if (ctr.inited(sym)) {
      return true;
    }
  }
  std::cerr << "error: uninitialized variable " << sym << std::endl;
  return false;
}

inline string ir_next_0(parser_ll1 *p, syntax_tree *root) {
  return p->code_generate(root->get_child(0));
}

inline string ir_next_1(parser_ll1 *p, syntax_tree *root) {
  return p->code_generate(root->get_child(1));
}

inline string ir_next_0_1(parser_ll1 *p, syntax_tree *root) {
  p->code_generate(root->get_child(0));
  p->code_generate(root->get_child(1));
  return "";
}

inline string ir_var(parser_ll1 *p, syntax_tree *root) {
  const auto &sym = root->get_child(0)->type._token.content;
  if (!is_initialize(sym, p->ctr)) {
    p->exit(-1);
    return "error";
  }
  return sym;
}

inline string ir_num(parser_ll1 *p, syntax_tree *root) {
  // auto d = utility::str_to_double(root->get_child(0)->type._token.content);
  return root->get_child(0)->type._token.content;
}

inline string ir_anti(parser_ll1 *p, syntax_tree *root) {
  auto condition = p->code_generate(root->get_child(2));
  p->emit("!", condition, "", "");
  return condition;
}

inline string ir_assign(parser_ll1 *p, syntax_tree *root) {
  auto right = p->code_generate(root->get_child(1));
  auto left = p->code_generate(root->get_child(0));
  // TODO(Daniel) make declaration first
  //  if (p->ctr.get(left)) {
  //    std::cerr << "redefine of" << left << std::end;
  //    return "error";
  //  }
  p->ctr.define(left, "double");
  p->ctr.init(left);
  p->emit("=", right, "", left);
  return left;
}

/* expect order : if condition A else B */
inline string ir_if(parser_ll1 *p, syntax_tree *root) {
  auto condition = p->code_generate(root->get_child(0));
  p->emit("if", condition, "goto", std::to_string(p->quads.size() + 2));
  int false_condition = p->quads.size();
  p->emit("goto", "", "", "");
  auto ok = p->code_generate(root->get_child(1));
  int true_condition = p->quads.size();
  p->emit("goto", "", "", "");
  p->quads[false_condition].result = std::to_string(p->quads.size());
  p->code_generate(root->get_child(2));
  p->quads[true_condition].result = std::to_string(p->quads.size());
  return "";
}

inline string ir_while(parser_ll1 *p, syntax_tree *root) {
  int condition_line = p->quads.size();
  auto condition = p->code_generate(root->get_child(0));
  p->emit("if", condition, "goto", std::to_string(p->quads.size() + 2));
  p->emit("goto", "", "", std::to_string(condition_line));
  int break_condition = p->quads.size();
  p->emit("goto", "", "", "");
  p->code_generate(root->get_child(1));
  p->quads[break_condition].result = std::to_string(p->quads.size());
  return "";
}
}
