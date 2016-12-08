#include <cwchar>
#include <functional>
#include <map>
#include <set>
#include <string>
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
using std::shared_ptr;

typedef vector<pair<char, char>> char_match;

struct fa_edge;
struct fa_state;

struct fa_state {
  set<fa_edge *> in_edges;
  set<fa_edge *> out_edges;
  bool isFinal;
  fa_state() = default;
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
  vector<shared_ptr<fa_state>> statues;
  vector<shared_ptr<fa_edge>> edges;
  fa_state *current;
  fa_state *entry;

  void split(fa_edge *edge_to_split);
  void closure(fa_edge *closure_edge, int closure_mark_index);
  void connection(fa_edge *connect_edge, int right_start_index);
  void parallel(fa_edge *parallel_edge, int parallel_index);

public:
  void dfs();
  void add_regular(string regular_expression);
  void make_deterministic();
  int test(string word);
  string step(char input);
  fa_state *create_state();
  fa_edge *create_edge(fa_state *start, fa_state *end, string regex_str);
  finite_automation();
};

class lexer {
  string src;

public:
  explicit lexer(string src) : src(src) {}
};
