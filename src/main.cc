#include "../Grapher/Graphing.hpp"
#include "Tokenizer.hpp"
#include <Types.hpp>
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <unordered_map>

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " \"<expression>\"\n";
    return EXIT_FAILURE;
  }

  // Tokenizer::tokenize(argv[1]);

  auto grapher = Grapher();
  grapher.draw(argv[1]);
}