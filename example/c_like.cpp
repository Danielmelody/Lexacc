#include "finite_automation.hpp"
#include "preprocess.hpp"
#include <iostream>
#include <string>
#include <vector>

void print_match(int status, const int expect) {
  if (status == expect) {
    std::cout << "SUCCESS , token type " << expect << std::endl;
    return;
  }
  std::cout << "FAILED" << std::endl;
}

std::vector<string> split_trick(string input) {
  std::vector<string> results;
  int last_index(0);
  for (int i = 0; i < input.length() - 1; ++i) {
    if ((input[i] == '+' || input[i] == '-') &&
        ((input[i] == '+' || input[i] == '-') || i == 0)) {
      results.push_back(input.substr(last_index, i + 1 - last_index));
      last_index = i + 1;
      results.push_back(input.substr(last_index, i + 2));
      last_index = i;
      i++;
    }
  }
  results.push_back(input.substr(last_index, input.length() - last_index));
  return results;
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
  fa.add_regular("\\l(\\l|\\d)*", 10);
  fa.add_regular("(+|-)?\\d\\d*(.\\d\\d*)?(e(+|-)?\\d\\d*)?", 20);
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
  string match_test = "while";
  auto tokens = fa.match(match_test);
  for (auto str : tokens) {
    std::cout << "(" << str.content << ", " << str.state_code << ")"
              << std::endl;
  }
}
