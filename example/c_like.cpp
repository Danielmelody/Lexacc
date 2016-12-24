#include "finite_automation.hpp"
#include "parser.hpp"
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
    if (((input[i] == '+' || input[i] == '-' || input[i] == '*' ||
          input[i] == '/') &&
         (input[i + 1] == '+' || input[i + 1] == '-'))) {
      results.push_back(input.substr(last_index, i - last_index));
      last_index = i;
      results.push_back(input.substr(last_index, 1));
      last_index = i + 1;
      i++;
    } else if ((input[i] == '+' || input[i] == '-') &&
               (input[i + 1] >= '0' || input[i + 1] <= '9') && i != 0 &&
               input[i - 1] != 'e' && input[i - 1] != '(') {
      results.push_back(input.substr(last_index, i - last_index));
      results.push_back(input.substr(i, 1));
      last_index = i + 1;
      i++;
    }
  }
  results.push_back(input.substr(last_index, input.length() - last_index));
  return results;
}

vector<symbol> to_symbol(const vector<token> &ts) {
  vector<symbol> symbols;
  for (auto t : ts) {
    symbols.push_back(symbol(t));
  }
  return symbols;
}

void check(parser_ll1 &p, vector<symbol> ss) {
  if (p.check(ss)) {
    std::cout << "grammar is correct" << std::endl;
  } else {
    std::cout << "grammar is wrong" << std::endl;
  }
}

double str_to_double(string str) {
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

int main(int argc, char const *argv[]) {
  finite_automation fa;
  fa.define_token(token_type("main", 1));
  fa.define_token(token_type("int", 2));
  fa.define_token(token_type("float", 3));
  fa.define_token(token_type("double", 4));
  fa.define_token(token_type("char", 5));
  fa.define_token(token_type("if", 6));
  fa.define_token(token_type("else", 7));
  fa.define_token(token_type("do", 8));
  fa.define_token(token_type("while", 9));
  fa.define_token(token_type("\\l(\\l|\\d)*", 10));
  fa.define_token(token_type("(+|-)?\\d\\d*(.\\d\\d*)?(e(+|-)?\\d\\d*)?", 20));
  fa.define_token(token_type("=", 21));
  fa.define_token(token_type("+", 22));
  fa.define_token(token_type("-", 23));
  fa.define_token(token_type("*", 24));
  fa.define_token(token_type("/", 25));
  fa.define_token(token_type("(", 26));
  fa.define_token(token_type(")", 27));
  fa.define_token(token_type("{", 28));
  fa.define_token(token_type("}", 29));
  fa.define_token(token_type(";", 30));

  fa.make_deterministic();
  // fa.dfs();
  char input[1000];

  vector<token> tokens;
  std::cout << "input text stream:" << std::endl;
  scanf("%[^$]s", input);
  string match_test = input;
  auto strs = split_trick(match_test);
  for (auto match : strs) {
    auto tokens_ = fa.match(match);
    for (auto &t : tokens_) {
      tokens.push_back(t);
    }
  }

  for (auto t : tokens) {
    std::cout << t.type.state_code << ", ";
    if (t.type.state_code == 20) {
      printf("%e, ", str_to_double(t.content));
    }
    std::cout << t.content << std::endl;
  }

  parser_ll1 p;
  symbol program("program");
  symbol main_arg("main_arg");
  symbol block("block");
  symbol statement_list("statement_list");
  symbol statement_list_right("statement_list_right");
  symbol statement("statement");
  symbol assign_statement("statement");
  symbol expression("expression");
  symbol expression_right("expression_right");
  symbol item("item");
  symbol item_right("item_right");
  symbol factor("factor");
  symbol var(fa[10]);
  symbol num(fa[20]);
  symbol plus(fa["+"]);
  symbol sub(fa["-"]);
  symbol mul(fa["*"]);
  symbol divs(fa["/"]);
  vector<symbol> empty_symbols;
  // p.add_grammar(program << fa["main"] + fa["("] + main_arg + fa[")"] +
  // block);
  // p.add_grammar(main_arg << empty_symbols);
  // p.add_grammar(block << fa["{"] + statement_list + fa["}"]);
  // p.add_grammar(statement_list << statement + fa[";"] +
  // statement_list_right);
  // p.add_grammar(statement << assign_statement);
  // p.add_grammar(assign_statement << var + fa["="] + expression);
  p.add_grammar(expression << item + expression_right);
  p.add_grammar(expression_right << plus + item + expression_right);
  p.add_grammar(expression_right << sub + item + expression_right);
  p.add_grammar(expression_right << empty_symbols);
  p.add_grammar(item << factor + item_right);
  p.add_grammar(item_right << mul + factor + item_right);
  p.add_grammar(item_right << divs + factor + item_right);
  p.add_grammar(item_right << empty_symbols);
  p.add_grammar(factor << fa["("] + expression + fa[")"]);
  p.add_grammar(factor << fa["-"] + fa["("] + expression + fa[")"]);
  p.add_grammar(factor << fa["+"] + fa["("] + expression + fa[")"]);
  p.add_grammar(factor << var);
  p.add_grammar(factor << num);
  p.set_start(expression);
  p.build();
  auto symbol_stream = to_symbol(tokens);
  check(p, symbol_stream);
}
