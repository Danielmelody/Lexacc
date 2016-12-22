#include "token.hpp"

const token_type token_type::error_token = token_type("$", -1);
const token_type token_type::start_token = token_type("$s", -2);
