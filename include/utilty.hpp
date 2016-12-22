#ifndef __UTILTY_HPP_
#define __UTILTY_HPP_

#include "parser.hpp"
#include "token.hpp"
#include <vector>

using std::vector;

namespace utility {
vector<symbol> &convert_tokens(vector<token> &tokens) {
  vector<symbol> symbols;
  for (auto t : tokens) {
    symbols.push_back(symbol(t));
  }
  return symbols;
}
}

#endif
