#include "lexer.hpp"
#include <regex>
#include <string>

using std::make_shared;

finite_automation::finite_automation() {
  memset(char_class, -1, max_charset_num);
  char_class[0] = -2;
}

int main(int argc, char const *argv[]) { return 0; }
