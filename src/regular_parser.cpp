#include "lexer.hpp"
#include <set>
#include <stack>
#include <string>
#include <vector>

using std::set;
using std::vector;
using std::stack;
using std::make_shared;
using std::make_pair;

static vector<set<char>> priority_table = {
    {
        '|',
    },
    {
        '^', '$',
    },

    {
        '*', '?', '+',
    },
    {
        '(',
    },
    {
        '\\',
    },
};

static char_match final_regex(wchar_t regex_char) {
  switch (regex_char) {
  case L'd':
    return {
        make_pair(L'0', L'9'),
    };
  case L'l':
    return {
        make_pair(L'a', L'z'), make_pair(L'A', L'Z'),
    };
  case L'w':
    return {
        make_pair(L'a', L'z'), make_pair(L'A', L'Z'), make_pair(L'0', L'9'),
    };
  }
  return {
      make_pair(regex_char, regex_char + 1),
  };
}

static int get_prioriry(char ch) {
  int priority(-1);
  for (int i = 0; i < priority_table.size(); ++i) {
    auto op_set = priority_table[i];
    bool broken_flag(false);
    for (auto op : op_set) {
      if (op == ch) {
        priority = i;
        broken_flag = true;
        break;
      }
    }
    if (broken_flag) {
      break;
    }
  }
  return priority;
}

void build_nfa(fa_state *entry, string regular_expression) {
  int max_prio(-1);
  int max_prio_index(-1);
  for (int i = 0; i < regular_expression.length(); ++i) {
    auto priority = get_prioriry(regular_expression[i]);
    if (priority > max_prio) {
      max_prio = priority;
      max_prio_index = i;
    }
  }
}

void finite_automation::split(fa_edge *edge_to_split) {
  int max_prio(-1);
  int max_prio_index(-1);
  for (int i = 0; i < edge_to_split->regex_str.length(); ++i) {
    auto priority = get_prioriry(edge_to_split->regex_str[i]);
    if (priority > max_prio) {
      max_prio = priority;
      max_prio_index = i;
    }
  }
  if (max_prio_index == -1) {
    // TODO(Daniel): connection operation or final state
  }

  char split_rule = edge_to_split->regex_str[max_prio_index];

  // TODO(Daniel): fill all ops;
  switch (split_rule) {
  case '|':
    break;
  case '?':
    break;
  case '*':
    break;
  case '(':
    break;
  }
}

void finite_automation::closure(fa_edge *origin) {
  auto sub_regex = origin->regex_str.substr(1);
  auto closure_state = create_state();
  auto closure_edge = create_edge(closure_state, closure_state, sub_regex);
  create_edge(closure_edge->start, closure_state, "");
  auto leave_closure = origin;
  leave_closure->start = closure_state;
  leave_closure->regex_str = "";
}

fa_edge *finite_automation::create_edge(fa_state *start, fa_state *end,
                                        string regex_str) {
  auto edge = make_shared<fa_edge>(start, end, regex_str);
  edges.push_back(edge);
  return edge.get();
}

fa_state *finite_automation::create_state() {
  auto state = make_shared<fa_state>();
  statues.push_back(state);
  return state.get();
}

void finite_automation::add_regular(string regular_expression) {
  auto entry = make_shared<fa_state>(0);
  auto current = entry;

  int index(0);
  stack<char> op_stack;
  stack<char> normal_stack;
}
