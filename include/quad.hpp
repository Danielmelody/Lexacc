#ifndef __COMPILER_QAUD_HPP_
#define __COMPILER_QAUD_HPP_

#include <string>

using std::string;

struct quad {
  string op;
  string arg1;
  string arg2;
  string result;
  quad(string op, string arg1, string arg2, string result)
      : op(op), arg1(arg1), arg2(arg2), result(result) {}
};

#endif
