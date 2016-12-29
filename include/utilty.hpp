#ifndef __UTILTY_HPP_
#define __UTILTY_HPP_

#include "parser.hpp"
#include "token.hpp"
#include <vector>

using std::vector;

namespace utility {
inline vector<symbol> convert_tokens(vector<token> &tokens) {
  vector<symbol> symbols;
  for (auto t : tokens) {
    symbols.push_back(symbol(t));
  }
  return symbols;
}

inline double str_to_double(string str) {
  double result = 0;
  double exp = 0;
  bool decimals = false;
  int deci_count = 0;
  bool scientific = false;
  int sign = 1;
  int sign_exp = 1;
  for (int i = 0; i < str.size(); ++i) {
    if (str[i] == '.') {
      decimals = true;
      continue;
    }
    if (str[i] == 'e') {
      scientific = true;
      decimals = false;
      continue;
    }
    if (!scientific) {
      if (!decimals) {
        if (str[i] == '-') {
          sign = -1;
          continue;
        }
        result *= 10;
        result += str[i] - '0';
      } else {
        deci_count++;
        double decimal_digit = str[i] - '0';
        for (int i = 0; i < deci_count; ++i) {
          decimal_digit /= 10.0;
        }
        result += decimal_digit;
      }
    } else {
      if (str[i] == '-') {
        sign_exp = -1;
        continue;
      }
      exp *= 10;
      exp += str[i] - '0';
    }
  }
  while (exp--) {
    if (sign_exp == 1) {
      result *= 10.0;
    } else {
      result /= 10.0;
    }
  }
  return result * sign;
}
}

#endif
