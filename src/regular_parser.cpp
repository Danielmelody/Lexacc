#include "lexer.hpp"
#include <algorithm>
#include <iostream>
#include <list>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

using std::set;
using std::vector;
using std::stack;
using std::make_shared;
using std::make_pair;
using std::list;
using std::unordered_map;

vector<set<char>> priority_table = {
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
        '(', ')',
    },
    {
        '\\',
    },
};

static unordered_map<char, int> priority_refers;

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

inline void mark_least(fa_edge *edge) {
  std::cout << "least edge: " << edge->regex_str << std::endl;
  for (auto range : get_regex_range(edge->regex_str[0])) {
    for (auto ch = range.first; ch < range.second; ++ch) {
      edge->acceptable_chars.insert(ch);
    }
  }
}

inline int get_prioriry(char ch) {
  auto iter = priority_refers.find(ch);
  if (iter != priority_refers.end()) {
    return iter->second;
  }
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
  priority_refers[ch] = priority;
  return priority;
}

inline int left_reduction(int op_index, const string &regex_str) {
  int left_end(0);
  int parentheses(0);
  int char_count(0);
  while (left_end < op_index) {
    if (regex_str[left_end] == '(') {
      if (char_count > 0) {
        break;
      }
      char_count++;
      parentheses++;
    } else if (regex_str[left_end] == ')') {
      parentheses--;
    } else if (parentheses == 0 &&
               get_prioriry(regex_str[left_end]) <= get_prioriry('a')) {
      if (get_prioriry(regex_str[left_end]) < get_prioriry('a')) {
        break;
      }
      if (get_prioriry(regex_str[left_end]) == get_prioriry('a')) {
        if (char_count > 0) {
          break;
        }
        char_count++;
      }
    }
    left_end++;
  }
  return left_end;
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
    end->in_edges.insert(this);
    this->end = end;
  }
  this->regex_str = regex_str;
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

finite_automation::finite_automation() { entry = create_state(); }

int finite_automation::test(string word) {
  std::cout << "test word " << word << std::endl;
  for (auto ch : word) {
    if ("$" == step(ch)) {
      return -1;
    }
  }
  return 0;
}

string finite_automation::step(char input) {
  if (current->isFinal) {
    std::cout << "reach final" << std::endl;
    return "$";
  }

  for (auto out : current->out_edges) {
    if (out->acceptable_chars.find(input) != out->acceptable_chars.end()) {
      current = out->end;
      std::cout << "input " << input << "\t|"
                << "regex:" << out->regex_str << std::endl;
      return out->regex_str;
    }
  }
  return "$";
}

void finite_automation::split(fa_edge *edge_to_split) {
  const int length = edge_to_split->regex_str.length();

  if (length <= 1) {
    mark_least(edge_to_split);
    return;
  }

  bool no_concat(false);

  int last_parentheses_index(0);
  int left_end = left_reduction(length, edge_to_split->regex_str);

  int min_prio(100);
  int min_prio_index(left_end);
  int parentheses(0);
  std::cout << "reduction: " << edge_to_split->regex_str.substr(0, left_end)
            << std::endl;
  if (edge_to_split->regex_str[0] == '(' &&
      edge_to_split->regex_str[left_end - 1] == ')' && left_end == length) {
    edge_to_split->regex_str = edge_to_split->regex_str.substr(1, length - 2);
    split(edge_to_split);
    return;
  }

  if (left_end == length) {
    no_concat = true;
    left_end = 0;
  }

  std::cout << "split: " << edge_to_split->regex_str.substr(left_end)
            << std::endl;
  int right_logic_char_num(0);
  for (int i = left_end; i < length; ++i) {
    auto ch = edge_to_split->regex_str[i];
    auto priority = get_prioriry(ch);
    if (ch == '(') {
      parentheses++;
      right_logic_char_num++;
    } else if (ch == ')') {
      parentheses--;
      last_parentheses_index = i;
    } else {
      if (priority == get_prioriry('a')) {
        right_logic_char_num++;
      }
    }
    if (right_logic_char_num == 1) {
      priority = get_prioriry('a');
    }
    if ((parentheses == 0 || right_logic_char_num == 1) &&
        priority < min_prio) {
      if (!no_concat || (no_concat && priority > get_prioriry('a'))) {
        min_prio = priority;
        min_prio_index = i;
      }
    }
  }
  if (min_prio == get_prioriry('a')) {
    connection(edge_to_split, left_end);
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
  std::cout << "closure:" << origin->regex_str << std::endl;
  auto sub_regex = origin->regex_str.substr(0, closure_mark_index);
  auto closure_state = create_state();
  create_edge(origin->start, closure_state, "");
  auto leave_closure = origin;
  leave_closure->set(closure_state, origin->end, "");
  auto closure_edge = create_edge(closure_state, closure_state, sub_regex);
  std::cout << "closure result:" << closure_edge->regex_str << std::endl;
  split(closure_edge);
}

void finite_automation::connection(fa_edge *connect_edge,
                                   int right_start_index) {
  std::cout << "connection: " << connect_edge->regex_str << std::endl;
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

void finite_automation::dfs() {
  std::cout << "=====dfs start=====" << std::endl;
  list<fa_state *> que;
  unordered_set<fa_state *> states;
  que.push_back(entry);
  states.insert(entry);
  while (!que.empty()) {
    auto current = que.front();
    que.pop_front();
    for (auto out_iter = current->out_edges.begin();
         out_iter != current->out_edges.end(); ++out_iter) {
      auto out = *out_iter;
      std::cout << "dfs: " << out->regex_str << std::endl;
      if (states.find(out->end) == states.end()) {
        que.push_back(out->end);
        states.insert(out->end);
      }
    }
  }
  std::cout << "=====dfs end=====" << std::endl;
}

void finite_automation::make_deterministic() {
  list<fa_state *> que;
  unordered_set<fa_state *> states;
  que.push_back(entry);
  states.insert(entry);
  while (!que.empty()) {
    auto current = que.front();
    que.pop_front();

    for (auto out_iter = current->out_edges.begin();
         out_iter != current->out_edges.end();) {
      auto out = *out_iter;
      if (out->regex_str == "") {
        current->out_edges.erase(out_iter);
        out->end->in_edges.erase(out);
        auto out_edges = out->end->out_edges;
        for (auto new_out_edge : out_edges) {
          new_out_edge->set(current, new_out_edge->end,
                            new_out_edge->regex_str);
        }
        auto in_edges = out->end->in_edges;
        for (auto new_in_edge : in_edges) {
          new_in_edge->set(new_in_edge->start, current, new_in_edge->regex_str);
        }
        out_iter = current->out_edges.begin();
      } else {
        out_iter++;
      }
    }
    for (auto out_iter = current->out_edges.begin();
         out_iter != current->out_edges.end(); ++out_iter) {
      auto out = *out_iter;
      if (states.find(out->end) == states.end()) {
        que.push_back(out->end);
        states.insert(out->end);
      }
    }
  }
}

void finite_automation::add_regular(string regular_expression) {
  auto end = create_state();
  current = entry;
  end->isFinal = true;
  auto first_edge = create_edge(entry, end, regular_expression);
  split(first_edge);
}
