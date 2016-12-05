#include <cwchar>
#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

using std::string;
using std::vector;
using std::function;
using std::pair;
using std::map;
using std::shared_ptr;

constexpr int max_charset_num = 65536;

typedef vector<pair<wchar_t, wchar_t>> char_match;

struct fa_edge;
struct fa_state;

struct fa_state {
  vector<fa_edge *> in_edges;
  vector<fa_edge *> out_edges;
  bool isFinal;
  fa_state() = default;
};

struct fa_edge {
  char_match match_index;
  fa_state *start;
  fa_state *end;
  string regex_str;
  fa_edge(fa_state *start, fa_state *end, string regex_str)
      : start(start), end(end), regex_str(regex_str) {}
};

class finite_automation {
  int char_class[max_charset_num];
  vector<shared_ptr<fa_state>> statues;
  vector<shared_ptr<fa_edge>> edges;

  void split(fa_edge *edge_to_split);
  void closure(fa_edge *closure_edge);

public:
  void add_regular(string regular_expression);
  fa_state *create_state();
  fa_edge *create_edge(fa_state *start, fa_state *end, string regex_str);
  finite_automation();
};

class lexer {
  string src;

public:
  explicit lexer(string src) : src(src) {}
};
