#include "context_analyzer.hpp"
#include <cassert>
#include <string>

const sym_information *context_analyzer::get(string sym) const {
  auto current = l_values.begin();
  while (current != l_values.end()) {
    auto find = current->find(sym);
    if (find != current->end()) {
      return &find->second;
    }
    current++;
  }
  return nullptr;
}

string context_analyzer::temp() {
  return last_r_letter + std::to_string(last_r_digit++);
}

void context_analyzer::define(string sym, string type) {
  l_values.back().insert({sym, sym_information(type)});
}

void context_analyzer::init(string sym) {
  auto iter = l_values.back().find(sym);
  assert(iter != l_values.back().end());
  iter->second.initialized = true;
}

bool context_analyzer::defined(string sym) const {
  auto current = l_values.begin();
  while (current != l_values.end()) {
    auto find = current->find(sym);
    if (find != current->end()) {
      return true;
    }
    current++;
  }
  return false;
}

bool context_analyzer::inited(string sym) const {
  auto info = get(sym);
  assert(info);
  return info->initialized;
}
