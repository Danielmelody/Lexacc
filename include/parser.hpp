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
  string str;
  bool terminal;
  bool operator==(const symbol &rhs) const {
    return str == rhs.str && terminal == rhs.terminal;
  }

  pair<symbol, vector<symbol>> operator=(const vector<symbol> &rhs) {
    assert(!terminal);
    return make_pair(*this, rhs);
  }

  explicit symbol(string str, bool terminal = false)
      : str(str), terminal(terminal) {}
};

namespace std {
template <> struct hash<symbol> {
  std::size_t operator()(const symbol &k) const {
    using std::size_t;
    using std::hash;
    using std::string;
    return hash<string>()(k.str);
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

public:
  void add_grammar(pair<symbol, vector<symbol>>);
  void build();
};
