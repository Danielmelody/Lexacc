#include "context_analyzer.hpp"
#include <string>

string context_analyzer::temp() {
  return last_r_letter + std::to_string(last_r_digit++);
}
