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

struct syntax_rule;

struct symbol {
  const string str;
  const bool terminal;
  const int token_code;
  const static symbol epsilon;

  bool operator==(const symbol &rhs) const;

  syntax_rule operator<<(const vector<symbol> &rhs);

  syntax_rule operator<<(const symbol &rhs);

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

/**
 * quad struct definition:
 * {a} {operation} {b}
 */
struct syntax_rule {
  const symbol start;
  const vector<symbol> generation;
  vector<int> operation = {0, 1, 2};
  function<void(syntax_rule *self)> attribute_func;
  inline symbol operator[](int index) { return generation[index]; }
  const inline syntax_rule &attribute(vector<int> operation) {
    this->operation = operation;
    return *this;
  }
  syntax_rule(const symbol start, const vector<symbol> generation)
      : start(start), generation(generation) {}
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
