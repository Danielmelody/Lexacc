#include "lexer.hpp"
#include <iostream>
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
        // character priority is here
    },
    {
        '^', '$',
    },

    {
        '*', '?', '+',
    },
    {
        '\\',
    },
};

static char_match get_regex_range(char regex_char) {
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

static void mark_least(fa_edge *edge) {
  for (auto range : get_regex_range(edge->regex_str[0])) {
    for (auto ch = range.first; ch < range.second; ++ch) {
      edge->acceptable_chars.insert(ch);
    }
  }
}

fa_edge::fa_edge(fa_state *start, fa_state *end, string regex_str)
    : start(start), end(end), regex_str(regex_str) {}

void fa_edge::set(fa_state *start, fa_state *end, string regex_str) {
  if (this->start != start) {
    if (this->start) {
      this->start->out_edges.erase(this);
    }
    start->out_edges.insert(this);
    this->start = start;
  }
  if (this->end != end) {
    if (this->end) {
      this->end->in_edges.erase(this);
    }
    start->in_edges.insert(this);
    this->end = end;
  }
  this->regex_str = regex_str;
}

static int get_prioriry(char ch) {
  int priority(1); // default priority is for chars
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

finite_automation::finite_automation() {}

int finite_automation::test(string word) {
  std::cout << "test word " << word << std::endl;
  for (auto ch : word) {
    step(ch);
  }
  return 0;
}

void finite_automation::step(char input) {
  if (current->isFinal) {
    std::cout << "reach final" << std::endl;
    return;
  }

  for (auto out : current->out_edges) {
    if (out->acceptable_chars.find(input) != out->acceptable_chars.end()) {
      current = out->end;
      std::cout << "input " << input << "\t|"
                << "regex:" << out->regex_str << std::endl;
      break;
    }
  }
}

void finite_automation::split(fa_edge *edge_to_split) {
  std::cout << "split: " << edge_to_split->regex_str << std::endl;
  auto length = edge_to_split->regex_str.length();

  if (length <= 1) {
    mark_least(edge_to_split);
    return;
  }

  if (edge_to_split->regex_str[0] == '(' &&
      edge_to_split->regex_str[length - 1] == ')') {
    edge_to_split->regex_str = edge_to_split->regex_str.substr(1, length - 2);
    std::cout << edge_to_split->regex_str << std::endl;
    split(edge_to_split);
    return;
  }
  int min_prio(100);
  int min_prio_index(-1);
  int parentheses(0);
  int last_parentheses_index(0);
  for (int i = 0; i < length; ++i) {
    auto ch = edge_to_split->regex_str[i];
    if (ch == '(') {
      parentheses++;
      continue;
    }
    if (ch == ')') {
      parentheses--;
      last_parentheses_index = i;
      continue;
    }
    auto priority = get_prioriry(ch);
    if (parentheses == 0 && priority < min_prio) {
      min_prio = priority;
      min_prio_index = i;
    }
  }
  if (min_prio == 1) {
    if (edge_to_split->regex_str[0] == '(') {
      connection(edge_to_split, last_parentheses_index + 1);
    } else {
      connection(edge_to_split, 1);
    }
    return;
  }

  char split_rule = edge_to_split->regex_str[min_prio_index];

  // TODO(Daniel): fill all ops;
  switch (split_rule) {
  case '|':
    parallel(edge_to_split, min_prio_index);
    break;
  case '?':
    break;
  case '*':
    closure(edge_to_split, min_prio_index);
    break;
  }
}

void finite_automation::parallel(fa_edge *parallel_edge, int parallel_index) {
  auto child_edge_1 = parallel_edge;
  auto child_edge_2 =
      create_edge(parallel_edge->start, parallel_edge->end,
                  parallel_edge->regex_str.substr(parallel_index + 1));
  child_edge_1->regex_str = parallel_edge->regex_str.substr(0, parallel_index);
  split(child_edge_1);
  split(child_edge_2);
}

void finite_automation::closure(fa_edge *origin, int closure_mark_index) {
  auto sub_regex = origin->regex_str.substr(closure_mark_index);
  auto closure_state = create_state();
  auto closure_edge = create_edge(closure_state, closure_state, sub_regex);
  create_edge(closure_edge->start, closure_state, "");
  auto leave_closure = origin;
  leave_closure->start = closure_state;
  leave_closure->regex_str = "";
  leave_closure->set(closure_state, origin->end, "");
  split(closure_edge);
}

void finite_automation::connection(fa_edge *connect_edge,
                                   int right_start_index) {
  auto right_regex = connect_edge->regex_str.substr(right_start_index);
  auto left_regex = connect_edge->regex_str.substr(0, right_start_index);
  auto left_state = create_state();
  auto child_left_edge = connect_edge;
  auto child_right_edge =
      create_edge(left_state, connect_edge->end, right_regex);
  child_left_edge->set(connect_edge->start, left_state, left_regex);
  split(child_left_edge);
  split(child_right_edge);
}

fa_edge *finite_automation::create_edge(fa_state *start, fa_state *end,
                                        string regex_str) {
  auto edge = make_shared<fa_edge>(start, end, regex_str);
  start->out_edges.insert(edge.get());
  end->in_edges.insert(edge.get());
  edges.push_back(edge);
  return edge.get();
}

fa_state *finite_automation::create_state() {
  auto state = make_shared<fa_state>();
  statues.push_back(state);
  return state.get();
}

void finite_automation::make_deterministic() {}

void finite_automation::add_regular(string regular_expression) {
  auto entry = create_state();
  auto end = create_state();
  current = entry;
  end->isFinal = true;
  auto first_edge = create_edge(entry, end, regular_expression);
  split(first_edge);
}
