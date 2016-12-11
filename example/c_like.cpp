#include "finite_automation.hpp"
#include "preprocess.hpp"
#include <iostream>
#include <string>

void print_match(int status, const int expect) {
  if (status == expect) {
    std::cout << "SUCCESS , token type " << expect << std::endl;
    return;
  }
  std::cout << "FAILED" << std::endl;
}

int main(int argc, char const *argv[]) {
  finite_automation fa;
  fa.add_regular("main", 1);
  fa.add_regular("int", 2);
  fa.add_regular("float", 3);
  fa.add_regular("double", 4);
  fa.add_regular("char", 5);
  fa.add_regular("if", 6);
  fa.add_regular("else", 7);
  fa.add_regular("do", 8);
  fa.add_regular("while", 9);
  fa.add_regular("l(l|d)*", 10);
  // fa.add_regular("(+|-)?dd*(.dd*)?(e(+|-)?dd*)?", 20);
  fa.add_regular("=", 21);
  fa.add_regular("+", 22);
  fa.add_regular("-", 23);
  fa.add_regular("*", 24);
  fa.add_regular("/", 25);
  fa.add_regular("(", 26);
  fa.add_regular(")", 27);
  fa.add_regular("{", 28);
  fa.add_regular("}", 29);
  fa.add_regular(";", 30);
  fa.make_deterministic();
  fa.dfs();
  string match_test = "while32";
  // match_test = process_streams(match_test);
  // std::cin >> match_test;
  auto strs = fa.match(match_test);
  for (auto str : strs) {
    std::cout << str.content << std::endl;
  }
}
