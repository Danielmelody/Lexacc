#include "lexer.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
  finite_automation nfa;
  nfa.add_regular("(a|bb)b");
  nfa.test("aaa");
}
