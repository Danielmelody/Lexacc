#ifndef __COMPILER_TOKEN_
#define __COMPILER_TOKEN_
#include <string>

using std::string;

struct token_type {
  const static token_type error_token;
  const static token_type start_token;
  const string regex;
  const int state_code;
  bool operator==(const token_type &rhs) const {
    return regex == rhs.regex && state_code == rhs.state_code;
  }
  token_type(string regex, int state_code)
      : regex(regex), state_code(state_code) {}
  token_type(const token_type &rhs)
      : regex(rhs.regex), state_code(rhs.state_code) {}
};

struct token {
  const string content;
  const token_type &type;
  token(string content, const token_type &type)
      : content(content), type(type) {}
};
#endif
