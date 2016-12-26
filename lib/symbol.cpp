#include "symbol.hpp"

bool symbol::operator==(const symbol &rhs) const {
  return str == rhs.str && terminal == rhs.terminal;
}

syntax_rule symbol::operator<<(const vector<symbol> &rhs) {
  assert(!terminal);
  return syntax_rule(*this, rhs);
}

syntax_rule symbol::operator<<(const symbol &rhs) {
  assert(!terminal);
  auto s = {rhs};
  return syntax_rule(*this, s);
}
