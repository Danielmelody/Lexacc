#include "parser.hpp"
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

using std::queue;

void parser_ll1::add_grammar(pair<symbol, vector<symbol>> grammar) {
  rules[grammar.first].push_back(grammar.second);
}

void parser_ll1::build() {
  for (auto rule : rules) {
    queue<symbol> q;
    auto non_ternimal_symbol = rule.first;
    q.push(rule.second[0][0]);
    while (!q.empty()) {
      auto current = q.front();
      if (current.terminal) {
        first[non_ternimal_symbol].insert(current);
      } else {
        for (auto next : (*(rules.find(current))).second) {
          q.push(next[0]);
        }
      }
    }
  }
  for (auto f_set : first) {
    for (auto f : f_set.second) {
      std::cout << f.str << ", ";
    }
  }
  std::cout << std::endl;
}
