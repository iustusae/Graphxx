#include "Tokenizer.hpp"
#include <fmt/core.h>

#include <Types.hpp>
#include <cmath>

int main() {
  //   types::Divide add{};
  //   add.left = new types::ValueNode(2.0);
  //   add.right = new types::ValueNode(0);

  //   add.print();                               // Print the expression
  //   fmt::print("\nResult: {}\n", add.eval());  // Evaluate and print the
  //   result

  //   // Clean up dynamically allocated memory
  //   delete add.left;
  //   delete add.right;
  // Tokenizer::tokenize("0.23 + 25889.333");
  fmt::println("{}", Tokenizer::evaluate<long double>(
                         "(7 + 3) * (12 - 4) / (5 + 2) + 15 - (6 * 3) + (8 / "
                         "2) * (9 - 1) - (4 + 6) * 2 + 10 / (3 + 1) + 5 * (3 - "
                         "1) - (2 * 4) + (18 / 3) - (5 + 7) * 2"));
}