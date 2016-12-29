#include "symbol.hpp"

bool symbol::operator==(const symbol &rhs) const {
  return str == rhs.str && terminal == rhs.terminal;
}

syntax_rule *symbol::operator<<(const vector<symbol> &rhs) {
  assert(!terminal);
  return new syntax_rule(*this, rhs);
}

syntax_rule *symbol::operator<<(const symbol &rhs) {
  assert(!terminal);
  auto s = {rhs};
  return new syntax_rule(*this, s);
}
