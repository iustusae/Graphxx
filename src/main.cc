#include "Tokenizer.hpp"
#include <fmt/core.h>

#include <Types.hpp>
#include <cmath>
#include <unordered_map>

int main(int argc, char **argv) {

  assert(argc == 2);
  std::unordered_map<char, double> var_values{
      {'x', 1.0},
  };

  while (var_values.at('x') <= 10) {
    fmt::print("x = {}, f(x) = {}\n", var_values.at('x'),
               Tokenizer::evaluate<double>(argv[1], var_values));
    var_values.at('x') += 0.1;
  }
}