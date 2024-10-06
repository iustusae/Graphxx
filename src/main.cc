#include <fmt/core.h>

#include <FunctionParser.hpp>
#include <Types.hpp>
#include <cmath>

int main() {
  types::Divide add{};
  add.left = new types::ValueNode(2.0);
  add.right = new types::ValueNode(0);

  add.print();                               // Print the expression
  fmt::print("\nResult: {}\n", add.eval());  // Evaluate and print the result

  // Clean up dynamically allocated memory
  delete add.left;
  delete add.right;

  using namespace types;
  ;
}
