#ifndef __COMPILER_SYMBOL_TABLE_HPP_
#define __COMPILER_SYMBOL_TABLE_HPP_

#include <list>
#include <string>
#include <unordered_map>

using std::unordered_map;
using std::string;
using std::list;

struct sym_information {
  string type_str;
  bool initialized;
  explicit sym_information(string type_str)
      : type_str(type_str), initialized(false) {}
};

class context_analyzer {
  list<unordered_map<string, sym_information>> l_values;
  unordered_map<string, sym_information> r_values;
  string last_r_letter;
  int64_t last_r_digit;

public:
  /**
   * generate a temp variable name without conflicts
   */
  string temp();

  /**
   * define a symbol in current context, but not initialized
   */
  void define(string sym, string type);

  /**
   * init symbol, expect a pre-defined symbol as parameter
   */
  void init(string sym);

  /**
   * test if a symbol has been defined
   * in current or parent context, or parent-parent, etc;
   */
  bool defined(string sym) const;

  /**
   * test if a symbol has been init
   * in current or parent context, or parent-parent, etc;
   */
  bool inited(string sym) const;

  /**
   * get a symbol info from name, if not defined, return null
   */
  const sym_information *get(string sym) const;

  context_analyzer() : last_r_letter("T"), last_r_digit(1) {
    l_values.push_back({});
  }
};

#endif
