#ifndef __COMPILER_PARSER_HPP_
#define __COMPILER_PARSER_HPP_

#include "context_analyzer.hpp"
#include "quad.hpp"
#include "symbol.hpp"
#include "syntax_tree.hpp"
#include "token.hpp"
#include <cassert>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using std::string;
using std::vector;
using std::function;
using std::unordered_map;
using std::unordered_multimap;
using std::unordered_set;
using std::pair;
using std::make_pair;

/**
 * configable parser for LL(1) grammar
 * use predict table
 */
class parser_ll1 {
  unordered_map<symbol, unordered_map<symbol, syntax_rule>> predict_table;
  unordered_map<symbol, unordered_set<symbol>> first;
  unordered_map<symbol, unordered_set<symbol>> follow;
  unordered_map<symbol, vector<syntax_rule>> rules;

  unordered_set<symbol> symbols;
  unordered_set<symbol> combine_symbols;
  symbol *start;
  unordered_set<string> epsilon_closure_visited;
  unordered_set<string> epsilon_symbols;
  unordered_set<string> non_epsilon_symbols;
  unordered_set<string> follow_visited;

  bool epsilon_closure(const symbol &sym, const symbol &start);
  void setup_follow();
  void setup_one_follow(const symbol &start, const symbol &current);

  int state;

public:
  vector<quad> quads;
  context_analyzer ctr;
  void emit(string operation, string arg1, string arg2, string result);
  void set_start(symbol start);
  void add_grammar(const syntax_rule *rule);
  void combinable(const symbol sym);
  void build();
  string code_generate(syntax_tree *root);
  void print_ir();
  std::shared_ptr<syntax_tree> parse(vector<symbol> &inputs);
  void exit(int code);
  parser_ll1() : state(0) {}
};

#endif
