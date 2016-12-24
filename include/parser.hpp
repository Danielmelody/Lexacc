#ifndef __COMPILER_SYMBOL_HPP_
#define __COMPILER_SYMBOL_HPP_

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

struct symbol {
  const string str;
  const bool terminal;
  const int token_code;
  const static symbol epsilon;

  bool operator==(const symbol &rhs) const {
    return str == rhs.str && terminal == rhs.terminal;
  }

  pair<symbol, vector<symbol>> operator<<(const vector<symbol> &rhs) {
    assert(!terminal);
    return make_pair(*this, rhs);
  }

  pair<symbol, vector<symbol>> operator<<(const symbol &rhs) {
    assert(!terminal);
    auto s = {rhs};
    return make_pair(*this, s);
  }

  symbol(const symbol &copy)
      : str(copy.str), terminal(copy.terminal), token_code(copy.token_code) {}

  symbol(const symbol &&move)
      : str(move.str), terminal(move.terminal), token_code(move.token_code) {}

  explicit symbol(string str) : str(str), terminal(false), token_code(-1) {}
  explicit symbol(token_type t)
      : str(t.regex), terminal(true), token_code(t.state_code) {}
  explicit symbol(token t)
      : str(t.type.regex), terminal(true), token_code(t.type.state_code) {}
};

vector<symbol> operator+(vector<symbol> lhs, const symbol &rhs);

vector<symbol> operator+(const symbol &lhs, const symbol &rhs);

namespace std {
template <> struct hash<symbol> {
  std::size_t operator()(const symbol &k) const {
    using std::hash;
    return hash<string>()(k.str);
  }
};

template <> struct hash<vector<symbol>> {
  std::size_t operator()(const vector<symbol> &k) const {
    using std::hash;
    auto seed = k.size();
    for (auto &i : k) {
      seed ^= hash<string>()(i.str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};
}

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
  void add_grammar(pair<symbol, vector<symbol>>);
  void build();
  bool check(vector<symbol> &inputs);
};

#endif
