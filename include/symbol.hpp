#ifndef __COMPILER_SYMBOL_HPP_
#define __COMPILER_SYMBOL_HPP_

#include "token.hpp"
#include <cassert>
#include <functional>
#include <string>
#include <utility>
#include <vector>

using std::string;
using std::vector;
using std::pair;
using std::function;
using std::make_pair;

class parser_ll1;
struct syntax_tree;
struct syntax_rule;

struct symbol {
  const string str;
  const bool terminal;
  const int token_code;
  token _token;
  const bool combine_to_parent;
  const static symbol epsilon;

  bool operator==(const symbol &rhs) const;

  syntax_rule *operator<<(const vector<symbol> &rhs);

  syntax_rule *operator<<(const symbol &rhs);

  symbol(const symbol &copy)
      : str(copy.str), terminal(copy.terminal), token_code(copy.token_code),
        _token(copy._token), combine_to_parent(copy.combine_to_parent) {}

  symbol(const symbol &&move)
      : str(move.str), terminal(move.terminal), token_code(move.token_code),
        _token(move._token), combine_to_parent(move.combine_to_parent) {}

  explicit symbol(string str, bool combine_to_parent = false)
      : str(str), terminal(false), token_code(-1), _token(token::empty),
        combine_to_parent(combine_to_parent) {}
  explicit symbol(token_type t)
      : str(t.regex), terminal(true), token_code(t.state_code),
        _token(token::empty), combine_to_parent(false) {}
  explicit symbol(const token _token)
      : str(_token.type.regex), terminal(true),
        token_code(_token.type.state_code), _token(_token),
        combine_to_parent(false) {}
};

/**
 * quad struct definition:
 * {a} {operation} {b}
 */

struct syntax_rule {
  const symbol start;
  const vector<symbol> generation;
  vector<int> operation = {0, 2, 1};
  function<string(parser_ll1 *p, syntax_tree *root)> action;
  inline symbol operator[](int index) { return generation[index]; }
  inline syntax_rule *attribute(vector<int> operation) {
    this->operation = operation;
    return this;
  }

  inline syntax_rule *
  act(function<string(parser_ll1 *p, syntax_tree *root)> a) {
    this->action = a;
    return this;
  }

  syntax_rule(const symbol start, const vector<symbol> generation)
      : start(start), generation(generation), action(nullptr) {}
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

#endif
