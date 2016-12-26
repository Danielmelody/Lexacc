#ifndef __COMPILER_PARSER_HPP_
#define __COMPILER_PARSER_HPP_

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
  unordered_map<symbol, unordered_map<symbol, vector<symbol>>> predict_table;
  unordered_map<symbol, unordered_set<symbol>> first;
  unordered_map<symbol, unordered_set<symbol>> follow;
  unordered_map<symbol, vector<vector<symbol>>> rules;

  unordered_set<symbol> symbols;
  symbol *start;
  unordered_set<const symbol *> epsilon_closure_visited;

  bool epsilon_closure(const symbol &sym, const symbol &start);
  void setup_follow();
  void setup_one_follow(const symbol &start);

public:
  void set_start(symbol start);
  void add_grammar(syntax_rule rule);
  void build();
  syntax_tree *parse(vector<symbol> &inputs);
};

#endif
