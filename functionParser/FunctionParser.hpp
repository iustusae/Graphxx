#include <fmt/format.h>

#include <cctype>
#include <cmath>
#include <cstring>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <strstream>
#include <termcolor/termcolor.hpp>
#include <unordered_map>
#include <utility>
#include <variant>

namespace Parser {

class MissingMatchingParenException : public std::exception {
 private:
  int m_idx{};
  std::string_view m_expr{};
  std::string m_what{};

  auto getCaretToMatchErrorPosition(int idx, bool to_match) -> std::string {
    std::ostringstream s{};
    for (int ctr = 0; ctr < idx; ++ctr) {
      s << " ";
    }

    if (to_match) {
      s << termcolor::red << termcolor::bold << "^ To match this parenthesis";
    } else {
      s << termcolor::red << termcolor::bold
        << "^ Does not have a closing parenthesis";
    }
    return s.str();
  }

 public:
  MissingMatchingParenException(int idx, const std::string_view expr,
                                bool to_match)
      : m_idx(idx),
        m_expr(expr),
        m_what(fmt::format(
            "Invalid Expression. Missing Parenthesis {}\n {}\n{}", m_idx,
            m_expr, getCaretToMatchErrorPosition(m_idx, to_match))) {}

  virtual const char *what() const noexcept override { return m_what.c_str(); }
};

enum Operators : char {
  Add = '+',
  Sub = '-',
  Div = '/',
  Pow = '^',
  Mult = '*',
  None = '\0'
};

static inline constexpr std::array<Operators, 5> ops = {Add, Sub, Div, Pow,
                                                        Mult};

const std::unordered_map<Operators, std::function<double(double, double)>>
    token_to_bin_op = {
        {Add, [](double a, double b) { return a + b; }},
        {Sub, [](double a, double b) { return a - b; }},
        {Div, [](double a, double b) { return a / b; }},
        {Mult, [](double a, double b) { return a * b; }},
        {Pow, [](double a, double b) { return std::pow(a, b); }}};

class FunctionParser {
 public:
  using TokenType = std::variant<double, Operators>;
  std::string m_function{};

  int operatorPriority(const char op) const {
    switch (op) {
      case Add:
      case Sub:
        return 1;
      case Mult:
      case Div:
        return 2;
      case Pow:
        return 3;
      default:
        return 0;
    }
  }
  void print_vec(const std::vector<TokenType> &vec) {
    std::cout << fmt::format("#tokens = {}\n", std::size(vec));
    for (const auto &token : vec) {
      std::visit(
          [](const auto &value) {
            std::cout << value << "\n";  // Print the value with a space
          },
          token);
    }
  }

  void cls(std::ostringstream &oss, int line) {
    // printf("oss was cleared here %i\n", line);

    oss = {};
  }

  auto checkIfParensAreAllMatched(const std::string_view expr)
      -> std::string_view {
    std::stack<std::pair<char, int>> stack{};
    int index = 0;
    for (const auto &c : expr) {
      if (c == '(') {
        stack.push(std::make_pair(c, index));
      } else if (c == ')') {
        if (stack.empty())
          throw MissingMatchingParenException(index, expr, true);
        stack.pop();
      }
      ++index;
    }

    if (!stack.empty())
      throw MissingMatchingParenException(stack.top().second, expr, false);

    return expr;
  }

 public:
  FunctionParser(const std::string_view function)
      : m_function(checkIfParensAreAllMatched(function)) {}

  void extractAllNumbersFromExpression() {
    std::ostringstream ss{};
    for (const char &c : m_function) {
      if (isdigit(c) || c == '.') {
        ss << c;
      } else {
        if (!ss.str().empty()) {
          std::cout << "number found: " << ss.str() << "\n";
          ss.str("");  // Clear the string stream
          ss.clear();  // Clear any error flags
        }
      }
    }
    if (!ss.str().empty()) {
      std::cout << "number found: " << ss.str() << "\n";
    }
  }
  bool isOperator(const char &c) const noexcept {
    return (std::find(std::cbegin(ops), std::cend(ops), c) != std::cend(ops));
  }
  std::string shuntingYardAlgorithm() {
    std::vector<std::string> output{};
    std::stack<char> operator_stack{};
    std::ostringstream oss{};
    char curr = 0;
    Operators prev = Operators::None;

    for (int i = 0; i < m_function.length(); ++i) {
      curr = m_function[i];
      if (isdigit(curr) || curr == '.') {
        oss << curr;
      } else if (isOperator(curr)) {
        if (oss.rdbuf()->in_avail() != 0) {
          double res = std::stod(oss.str());
          if (std::isnan(res))
            throw std::runtime_error(
                fmt::format("invalid number {}", oss.str()));
          else {
            output.push_back(oss.str());
            oss.clear();
          }
        } else {
          if (operator_stack.empty()) {
            operator_stack.push(curr);
          } else {
            if (curr == Operators::Mult or curr == Operators::Div) {
            }
          }
        }
      }
    }
  }
};

}  // namespace Parser
