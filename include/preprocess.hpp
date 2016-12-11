#ifndef __PRE_PROCESS__H__
#define __PRE_PROCESS__H__

#include <algorithm>
#include <string>

using std::string;

inline string remove_comments(string origin) {
  finite_automation fa;
  fa.add_regular("//.*", 1);
  return origin;
}

#endif
