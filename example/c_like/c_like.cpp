#include "actions.hpp"
#include "finite_automation.hpp"
#include "parser.hpp"
#include "utilty.hpp"
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

void code_generate(parser_ll1 &p, vector<symbol> ss) {
  auto st = p.parse(ss);
  if (st) {
    std::cout << "grammar is correct" << std::endl;
    p.code_generate(st.get());
  } else {
    std::cout << "grammar is wrong" << std::endl;
  }
}

int main(int argc, char const *argv[]) {
  std::ios_base::sync_with_stdio(false);
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
  fa.define_token(token_type("+", 22));
  fa.define_token(token_type("-", 23));
  fa.define_token(token_type("*", 24));
  fa.define_token(token_type("/", 25));
  fa.define_token(token_type("(", 26));
  fa.define_token(token_type(")", 27));
  fa.define_token(token_type("{", 28));
  fa.define_token(token_type("}", 29));
  fa.define_token(token_type(";", 30));
  fa.define_token(token_type(",", 31));
  fa.define_token(token_type("<", 32));
  fa.define_token(token_type(">", 33));
  fa.define_token(token_type("<=", 34));
  fa.define_token(token_type(">=", 35));
  fa.define_token(token_type("==", 36));
  fa.define_token(token_type("!=", 37));
  fa.define_token(token_type("!", 38));
  fa.define_token(token_type("=", 39));
  fa.define_token(token_type("&&", 40));
  fa.define_token(token_type("\\|\\|", 41));

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

  // for (auto t : tokens) {
  //   std::cout << t.type.state_code << ", ";
  //   if (t.type.state_code == 20) {
  //     printf("%e, ", utility::str_to_double(t.content));
  //   }
  //   std::cout << t.content << std::endl;
  // }

  parser_ll1 p;
  symbol program("program");
  symbol main_arg("main_arg");
  symbol block("block");
  symbol statement_list("statement_list");
  symbol statement("statement");
  symbol assign_statement("assign_statement");
  symbol if_statement("if_statement");
  symbol while_statement("while_statement");
  symbol do_statement("do_statement");
  symbol else_statement("else_statement");
  symbol bool_expression("bool_expression");
  symbol single_bool_expression("single_bool_expression");
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

  p.combinable(symbol("bool_right"));
  p.combinable(symbol("single_bool_right"));
  p.combinable(symbol("expression_right"));
  p.combinable(symbol("item_right"));
  p.combinable(symbol("factor_right"));
  p.add_grammar((program << fa["main"] + fa["("] + main_arg + fa[")"] + block)
                    ->attribute({4})
                    ->act(actions::ir_next_0));
  p.add_grammar((main_arg << empty_symbols)->act(actions::ir_next_0));
  p.add_grammar(main_arg << fa["char"] + fa["*"] + fa["*"] + fa[10] + fa[","] +
                                fa["int"] + fa[10]);
  p.add_grammar(((block << fa["{"] + statement_list + fa["}"])
                     ->attribute({0, 1})
                     ->act(actions::ir_next_1)));
  p.add_grammar((statement_list << statement + statement_list)
                    ->attribute({0, 1})
                    ->act(actions::ir_next_0_1));
  p.add_grammar(statement_list << empty_symbols);
  p.add_grammar(
      (statement << assign_statement)->attribute({0})->act(actions::ir_next_0));
  p.add_grammar((statement << if_statement)->act(actions::ir_next_0));
  p.add_grammar((statement << while_statement)->act(actions::ir_next_0));
  p.add_grammar((statement << do_statement)->act(actions::ir_next_0));
  p.add_grammar((if_statement << fa["if"] + bool_expression + symbol("branch") +
                                     else_statement)
                    ->attribute({1, 2, 3})
                    ->act(actions::ir_if));
  p.add_grammar((symbol("branch") << statement)->act(actions::ir_next_0));
  p.add_grammar((symbol("branch") << block)->act(actions::ir_next_0));

  p.add_grammar((else_statement << fa["else"] + symbol("branch"))
                    ->attribute({0, 1})
                    ->act(actions::ir_next_1));
  p.add_grammar(else_statement << empty_symbols);

  p.add_grammar(bool_expression
                << single_bool_expression + symbol("bool_right"));
  p.add_grammar(
      (bool_expression << fa["("] + bool_expression + fa[")"])->attribute({1}));
  p.add_grammar(
      (bool_expression << fa["!"] + bool_expression)->act(actions::ir_anti));
  p.add_grammar((symbol("bool_right")
                 << fa["&&"] + single_bool_expression + symbol("bool_right"))
                    ->act(actions::ir_next_0_1)
                    ->attribute({1, 2}));
  p.add_grammar((symbol("bool_right") << fa["\\|\\|"] + single_bool_expression +
                                             symbol("bool_right"))
                    ->act(actions::ir_next_0_1)
                    ->attribute({1, 2}));
  p.add_grammar(
      (symbol("bool_right") << empty_symbols)->act(actions::ir_next_0));

  p.add_grammar(single_bool_expression
                << expression + symbol("single_bool_right"));
  p.add_grammar(symbol("single_bool_right") << fa["<"] + expression);
  p.add_grammar(symbol("single_bool_right") << fa[">"] + expression);
  p.add_grammar(symbol("single_bool_right") << fa["<="] + expression);
  p.add_grammar(symbol("single_bool_right") << fa[">="] + expression);
  p.add_grammar(symbol("single_bool_right") << fa["=="] + expression);
  p.add_grammar(symbol("single_bool_right") << fa["!="] + expression);
  p.add_grammar(
      (symbol("single_bool_right") << empty_symbols)->act(actions::ir_next_0));

  p.add_grammar((assign_statement << var + fa["="] + expression + fa[";"])
                    ->act(actions::ir_assign));
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
  p.add_grammar((factor << var)->act(actions::ir_var));
  p.add_grammar((factor << num)->act(actions::ir_num));

  p.add_grammar((while_statement << fa["while"] + fa["("] + bool_expression +
                                        fa[")"] + symbol("branch"))
                    ->attribute({2, 4})
                    ->act(actions::ir_while));
  p.add_grammar((do_statement << fa["do"] + symbol("branch") + fa["while"] +
                                     fa["("] + bool_expression + fa[")"] +
                                     fa[";"])
                    ->attribute({4, 1})
                    ->act(actions::ir_while));

  p.set_start(program);
  p.build();
  auto symbol_stream = to_symbol(tokens);
  code_generate(p, symbol_stream);
  p.print_ir();
}
