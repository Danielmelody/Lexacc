#ifndef __COMPILER_FINITE_AUTOMATION_
#define __COMPILER_FINITE_AUTOMATION_

#include "parser.hpp"
#include "token.hpp"
#include <cwchar>
#include <functional>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using std::string;
using std::vector;
using std::function;
using std::pair;
using std::map;
using std::set;
using std::unordered_set;
using std::unordered_map;
using std::shared_ptr;
using std::stack;

typedef vector<pair<char, char>> char_match;

struct fa_edge;
struct fa_state;

struct fa_state {
  set<fa_edge *> in_edges;
  set<fa_edge *> out_edges;
  int type_index;
  bool isFinal;
  stack<set<fa_edge *>::iterator> decision_edges;

  explicit fa_state(int type_index = 0)
      : type_index(type_index), isFinal(false) {}
};

struct fa_edge {
  unordered_set<char> acceptable_chars;
  fa_state *start;
  fa_state *end;
  string regex_str;
  void set(fa_state *start, fa_state *end, string regex_str);
  fa_edge(fa_state *start, fa_state *end, string regex_str);
};

class finite_automation {
  vector<const token_type> token_types;
  unordered_map<int, int> token_types_int_map;
  unordered_map<string, int> token_types_string_map;
  vector<shared_ptr<fa_state>> statues;
  vector<shared_ptr<fa_edge>> edges;
  fa_state *entry;
  fa_state *last_final_state;
  stack<fa_state *> decision_state;
  string input_str;
  int current_index_in_input;
  int last_final_index;

  void reset();
  void split(fa_edge *edge_to_split);
  void transfer(fa_edge *transfer_edge, int closure_mark_index);
  void closure(fa_edge *closure_edge, int closure_mark_index);
  void optional(fa_edge *option_edge, int option_mark_index);
  void connection(fa_edge *connect_edge, int right_start_index);
  void parallel(fa_edge *parallel_edge, int parallel_index);

public:
  const token_type &get_token_type(string regex);
  const token_type &get_token_type(int index);
  void dfs();
  void define_token(const token_type t);
  void make_deterministic();
  vector<token> match(string sentence);
  char step();
  void next();
  fa_state *create_state(int type_index = 0);
  fa_edge *create_edge(fa_state *start, fa_state *end, string regex_str);

  finite_automation();

  template <class param_type> const symbol operator[](param_type param) {
    return symbol(get_token_type(param));
  }
};

#endif
