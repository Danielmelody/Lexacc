#ifndef __PRE_PROCESS__H__
#define __PRE_PROCESS__H__

#include <algorithm>
#include <string>

#include "finite_automation.hpp"

using std::string;

inline string remove_comments(string origin) {
  finite_automation fa;
  fa.define_token(token_type("//.*", 1));
  return origin;
}

#endif
