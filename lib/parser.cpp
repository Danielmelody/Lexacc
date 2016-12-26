#include "parser.hpp"
#include "symbol.hpp"
#include <iostream>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

using std::queue;
using std::stack;

const symbol symbol::epsilon = symbol("epsilon");

vector<symbol> operator+(const symbol &lhs, const symbol &rhs) {
  vector<symbol> symbols = {lhs, rhs};
  return symbols;
}

vector<symbol> operator+(vector<symbol> lhs, const symbol &rhs) {
  lhs.push_back(rhs);
  return lhs;
}

void parser_ll1::set_start(symbol start) {
  assert(symbols.find(start) != symbols.end());
  this->start = &start;
}

bool parser_ll1::epsilon_closure(const symbol &sym, const symbol &start) {
  if (sym == start) {
    epsilon_closure_visited = {&start};
  }
  auto &current = sym;
  for (auto &rule : rules[current]) {
    if (rule.empty()) {
      return true;
    } else {
      for (auto &next : rule) {
        if (epsilon_closure_visited.find(&sym) !=
                epsilon_closure_visited.end() &&
            !next.terminal) {
          auto epsilon_next = epsilon_closure(next, start);
          if (epsilon_next) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

void parser_ll1::add_grammar(pair<symbol, vector<symbol>> grammar) {
  rules[grammar.first].push_back(grammar.second);
  symbols.insert(grammar.first);
  for (auto sym : grammar.second) {
    symbols.insert(sym);
  }
}

void parser_ll1::build() {
  for (auto rule : rules) {
    queue<symbol> q;
    auto non_ternimal_symbol = rule.first;
    for (auto spreation : rule.second) {
      for (auto s : spreation) {
        if (s.terminal) {
          first[s].insert(s);
        }
      }
    }
    for (int i = 0; i < rule.second.size(); ++i) {
      if (!rule.second[i].empty()) {
        if (epsilon_closure(non_ternimal_symbol, non_ternimal_symbol)) {
          first[non_ternimal_symbol].insert(symbol::epsilon);
        }
        q.push(rule.second[i][0]);
        while (!q.empty()) {
          auto current = q.front();
          q.pop();
          if (current.terminal) {
            first[non_ternimal_symbol].insert(current);
          } else {
            for (auto next : (*(rules.find(current))).second) {
              q.push(next[0]);
            }
          }
        }
      }
    }
  }

  setup_follow();

  for (auto rule : rules) {
    auto non_ternimal_symbol = rule.first;
    for (auto spreation : rule.second) {
      if (!spreation.empty()) {
        auto first_set = first[spreation[0]];
        for (auto f : first_set) {
          for (auto s : spreation) {
            auto &resolve = predict_table[non_ternimal_symbol][f];
            resolve.push_back(s);
          }
        }
      }
    }
  }

  for (auto rule_set : rules) {
    for (auto rule : rule_set.second) {
      auto &follow_set = follow[rule_set.first];
      if (!rule.empty()) {
        auto &first_set = first[rule.front()];
        if (first_set.find(symbol::epsilon) != first_set.end()) {
          for (auto &fo : follow_set) {
            predict_table[rule_set.first].insert({fo, rule});
          }
        }
      } else {
        for (auto &fo : follow_set) {
          predict_table[rule_set.first].insert({fo, {}});
        }
      }
    }
  }

  // for (auto f_set : first) {
  //    std::cout << "first set of " << f_set.first.str << ": ";
  //   for (auto f : f_set.second) {
  //     std::cout << f.str << ", ";
  //   }
  //   std::cout << std::endl;
  // }
  // for (auto f_set : follow) {
  //    std::cout << "follow set of " << f_set.first.str << ": ";
  //   for (auto f : f_set.second) {
  //     std::cout << f.str << ", ";
  //   }
  //   std::cout << std::endl;
  // }
}

void parser_ll1::setup_follow() {
  follow[*start].insert(symbol("$"));
  setup_one_follow(*start);
  for (auto rule_set : rules) {
    for (auto rule : rule_set.second) {
      if (!rule.empty()) {
        for (int i = 0; i < rule.size() - 1; ++i) {
          auto &follow_by = first[rule[i + 1]];
          for (auto &fo : follow_by) {
            if (!(fo == symbol::epsilon)) {
              follow[rule[i]].insert(fo);
            }
          }
        }
      }
    }
  }
  for (auto rule_set : rules) {
    for (auto rule : rule_set.second) {
      if (!rule.empty()) {
        for (int i = 0; i < rule.size() - 1; ++i) {
          setup_one_follow(rule[i]);
        }
      }
    }
  }
}

void parser_ll1::setup_one_follow(const symbol &start) {
  if (start.terminal) {
    return;
  }
  auto nexts = rules[start];
  for (auto next : nexts) {
    if (!next.empty()) {
      if (!(next.back() == start)) {
        for (auto fo : follow[start]) {
          follow[next.back()].insert(fo);
        }
        setup_one_follow(next.back());
      }
      int last = next.size() - 1;
      while (last >= 0 && epsilon_closure(next[last], next[last])) {
        last--;
        if (!(next[last] == start)) {
          for (auto fo : follow[start]) {
            follow[next[last]].insert(fo);
          }
        }
      }
    }
  }
}

inline const symbol &get_next(int &read_index, const vector<symbol> &inputs) {
  read_index++;
  return inputs[read_index - 1];
}

syntax_tree *parser_ll1::parse(vector<symbol> &inputs) {
  using std::make_shared;
  unordered_map<symbol, syntax_tree *> trees;

  auto root = make_shared<syntax_tree>(*start);
  trees.insert({*start, root.get()});

  inputs.push_back(symbol("$"));
  stack<symbol> analysis_stack;
  analysis_stack.push(symbol("$"));
  analysis_stack.push(*start);
  int read_index = 0;
  while (read_index < inputs.size() && !analysis_stack.empty()) {
    auto rule = analysis_stack.top();
    analysis_stack.pop();
    auto &current = inputs[read_index];
    if (rule.terminal) {
      if (!(current == rule)) {
        std::cerr << current.str << "should be " << rule.str << std::endl;
        return nullptr;
      } else {
        read_index++;
      }
    } else {
      if (rule.str == "$") {
        if (current == rule) {
          return root.get();
        } else {
          return nullptr;
        }
      } else {
        if (predict_table[rule].find(current) == predict_table[rule].end()) {
          // if(analysis_stack.empty()) {
          std::cerr << current.str << "cannot be resolved by " << rule.str
                    << std::endl;
          return nullptr;
          //}
        }
        auto resolve = predict_table[rule][current];

        for (int i = resolve.size() - 1; i >= 0; --i) {
          trees[rule]->children.push_back(make_shared<syntax_tree>(resolve[i]));
          analysis_stack.push(resolve[i]);
        }
      }
    }
  }
  return nullptr;
}
