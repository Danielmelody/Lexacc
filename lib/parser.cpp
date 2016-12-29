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
    epsilon_closure_visited = {start.str};
  }
  auto current = sym;
  epsilon_closure_visited.insert(current.str);
  bool has_epsilon_rule = false;
  for (auto rule : rules[current]) {
    if (rule.generation.empty()) {
      has_epsilon_rule = true;
    } else {
      has_epsilon_rule = true;
      for (auto next : rule.generation) {
        if (epsilon_closure_visited.find(next.str) ==
                epsilon_closure_visited.end()) {
          auto epsilon_next = epsilon_closure(next, start);
          if (!epsilon_next) {
            has_epsilon_rule = false;
          }
        } else {
          if (epsilon_symbols.find(next.str) == epsilon_symbols.end()) {
            has_epsilon_rule = false;
          }
        }
      }
    }
  }
  if (has_epsilon_rule) {
    epsilon_symbols.insert(sym.str);
    return true;
  }

  return false;
}

void parser_ll1::add_grammar(const syntax_rule *grammar) {
  rules[grammar->start].push_back(*grammar);
  symbols.insert(grammar->start);
  for (auto sym : grammar->generation) {
    symbols.insert(sym);
  }
}

void parser_ll1::combinable(const symbol sym) { combine_symbols.insert(sym); }

void parser_ll1::build() {
  for (auto rule : rules) {
    auto non_ternimal_symbol = rule.first;
    if (epsilon_closure(non_ternimal_symbol, non_ternimal_symbol)) {
      auto &first_set = first[non_ternimal_symbol];
      first_set.insert(symbol::epsilon);
    }
  }
  for (auto rule : rules) {
    auto non_ternimal_symbol = rule.first;
    for (auto spreation : rule.second) {
      for (auto s : spreation.generation) {
        if (s.terminal) {
          first[s].insert(s);
        }
      }
    }
    for (int i = 0; i < rule.second.size(); ++i) {
      if (!rule.second[i].generation.empty()) {
        queue<symbol> q;
        q.push(rule.second[i].generation[0]);
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
    const auto non_ternimal_symbol = rule.first;
    for (auto spreation : rule.second) {
      if (non_ternimal_symbol.str == "bool_expression") {
        0;
      }
      if (!spreation.generation.empty()) {
        auto first_set = first[spreation.generation[0]];
        for (auto f : first_set) {
          predict_table[non_ternimal_symbol].insert({f, spreation});
        }
        if (first_set.find(symbol::epsilon) != first_set.end()) {
          auto follow_set = follow[non_ternimal_symbol];
          for (auto fo : follow_set) {
            predict_table[non_ternimal_symbol].insert({fo, spreation});
          }
        }
      } else {
        auto follow_set = follow[non_ternimal_symbol];
        for (auto fo : follow_set) {
          predict_table[non_ternimal_symbol].insert({fo, spreation});
        }
      }
    }
  }

//  for (auto f_set : first) {
//    std::cout << "first set of " << f_set.first.str << ": ";
//    for (auto f : f_set.second) {
//      std::cout << f.str << ", ";
//    }
//    std::cout << std::endl;
//  }
//  for (auto f_set : follow) {
//    std::cout << "follow set of " << f_set.first.str << ": ";
//    for (auto f : f_set.second) {
//      std::cout << f.str << ", ";
//    }
//    std::cout << std::endl;
//  }
}

void parser_ll1::setup_follow() {
  follow[*start].insert(symbol("$"));
  setup_one_follow(*start, *start);
  for (auto rule_set : rules) {
    for (auto rule : rule_set.second) {
      if (!rule.generation.empty()) {
        for (int i = 0; i < rule.generation.size() - 1; ++i) {
          auto &follow_by = first[rule[i + 1]];
          for (auto &fo : follow_by) {
            if (!(fo == symbol::epsilon)) {
              if (fo.str == "}") {
                0;
              }
              follow[rule[i]].insert(fo);
            }
          }
        }
      }
    }
  }
  for (auto rule_set : rules) {
    for (auto rule : rule_set.second) {
      if (!rule.generation.empty()) {
        for (int i = 0; i < rule.generation.size(); ++i) {
          setup_one_follow(rule[i], rule[i]);
        }
      }
    }
  }
}

void parser_ll1::setup_one_follow(const symbol &start, const symbol &current) {
  if (current.terminal) {
    return;
  }
  if (start == current) {
    follow_visited = {current.str};
  } else {
    follow_visited.insert(current.str);
  }
  auto nexts = rules[current];

  for (auto next : nexts) {

    if (!next.generation.empty()) {
      auto follow_start = follow[start];
      for (auto fo : follow_start) {
        follow[next.generation.back()].insert(fo);
      }
      if (follow_visited.find(next.generation.back().str) ==
          follow_visited.end()) {
        setup_one_follow(start, next.generation.back());
      }
      int last = next.generation.size() - 1;
      while (last > 0 && epsilon_closure(next[last], next[last])) {
        last--;
        if (!(next[last] == current) &&
            follow_visited.find(next[last].str) == follow_visited.end()) {
          auto follow_start = follow[start];
          for (auto fo : follow[start]) {
            follow[next[last]].insert(fo);
          }
          setup_one_follow(start, next[last]);
        }
      }
    }
  }
}

inline const symbol &get_next(int &read_index, const vector<symbol> &inputs) {
  read_index++;
  return inputs[read_index - 1];
}

shared_ptr<syntax_tree> parser_ll1::parse(vector<symbol> &inputs) {
  using std::make_shared;

  inputs.push_back(symbol("$"));
  stack<symbol> analysis_stack;
  analysis_stack.push(symbol("$"));
  analysis_stack.push(*start);

  shared_ptr<syntax_tree> root = make_shared<syntax_tree>(*start);
  stack<syntax_tree *> syntax_stack;
  syntax_stack.push(nullptr);
  syntax_stack.push(root.get());

  int read_index = 0;
  while (read_index < inputs.size() && !analysis_stack.empty()) {
    auto rule = analysis_stack.top();
    auto current_syn_tree = syntax_stack.top();
    syntax_stack.pop();
    analysis_stack.pop();
    auto &current = inputs[read_index];
    if (rule.terminal) {
      if (!(current == rule)) {
        std::cerr << current.str << "should be " << rule.str << std::endl;
        return nullptr;
      } else {
        current_syn_tree->type._token.content = current._token.content;
        read_index++;
      }
    } else {
      if (rule.str == "$") {
        if (current == rule) {
          return root;
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
        auto resolve = predict_table[rule].at(current);
        current_syn_tree->set_resolve(resolve);

        for (int i = 0; i < resolve.generation.size(); ++i) {
          auto next_syn = make_shared<syntax_tree>(resolve[i]);
          current_syn_tree->children.push_back(next_syn);
        }
        for (int i = resolve.generation.size() - 1; i >= 0; --i) {
          syntax_stack.push(current_syn_tree->children[i].get());
          analysis_stack.push(resolve[i]);
        }
      }
    }
  }
  return nullptr;
}
