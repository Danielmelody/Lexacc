#ifndef __COMPILER_TOKEN_
#define __COMPILER_TOKEN_
#include <string>

using std::string;

struct token {
  string content;
  int state_code;
  token(string content, int state_code)
      : content(content), state_code(state_code) {}
};
#endif
