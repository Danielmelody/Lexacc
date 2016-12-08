#include "finite_automation.hpp"
#include <iostream>

int main(int argc, char const *argv[]) {
  finite_automation nfa;
  nfa.add_regular("(a|b)*");
  nfa.make_deterministic();
  nfa.dfs();
  switch (nfa.test("abbab")) {
  case 0:
    std::cout << "SUCCESS" << std::endl;
    break;
  case -1:
    std::cout << "FAILED" << std::endl;
    break;
  }
}
