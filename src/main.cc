#include "Tokenizer.hpp"
#include <fmt/core.h>

#include <Types.hpp>
#include <cmath>

int main()
{
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
    fmt::println("{}", Tokenizer::evaluate("3 + 2 + 2"));
}