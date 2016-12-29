#ifndef __COMPILER_SYMBOL_TABLE_HPP_
#define __COMPILER_SYMBOL_TABLE_HPP_

#include <stack>
#include <string>
#include <unordered_set>

using std::unordered_set;
using std::string;
using std::stack;

class context_analyzer {
  stack<unordered_set<string>> l_values;
  unordered_set<string> r_values;
  string last_r_letter;
  int64_t last_r_digit;

public:
  string temp();
  context_analyzer() : last_r_letter("T"), last_r_digit(1) {}
};

#endif
