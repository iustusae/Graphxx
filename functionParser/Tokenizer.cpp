#include "Tokenizer.hpp"
#include "Logger.hpp"
#include <SFML/Graphics/VertexArray.hpp>
#include <algorithm>
#include <cctype>
#include <fmt/base.h>
#include <iostream>
#include <regex>
#include <termcolor/termcolor.hpp>
#include <type_traits>
#include <unordered_map>
#include <variant>
bool Tokenizer::isOperator(const Tokenizer::Operator c) {
  using namespace Tokenizer;
  switch (c) {
  case Operator::Sum:
  case Operator::Sub:
  case Operator::Div:
  case Operator::Mult:
  case Operator::Pow:
  case Operator::LParen:
  case Operator::RParen:
  case Operator::Comma:
    return true;
  case Operator::None:
  default:
    return false;
  }
}

std::ostream &Tokenizer::operator<<(std::ostream &out,
                                    const Tokenizer::Operator &op) {
  out << static_cast<char>(op);
  return out;
}

std::ostream &Tokenizer::operator<<(std::ostream &out,
                                    const Tokenizer::UnaryFunction &func) {
  out << func.name;
  return out;
}

std::ostream &Tokenizer::operator<<(std::ostream &out,
                                    const Tokenizer::BinaryFunction &func) {
  out << func.name;
  return out;
}

std::ostream &Tokenizer::operator<<(std::ostream &out,
                                    const Tokenizer::Variable &var) {
  out << fmt::format("[{} = {}]", var.name, var.value);

  return out;
}

std::ostream &Tokenizer::operator<<(std::ostream &out,
                                    const std::queue<TokenType> &q) {
  auto cp = q;

  while (not cp.empty()) {
    cp.pop();
  }

  return out;
}
bool Tokenizer::isNumber(const TokenType &token) {
  return std::holds_alternative<double>(token);
}
bool Tokenizer::isOperator(const TokenType &token) {
  return std::holds_alternative<Operator>(token);
}
bool Tokenizer::isFunction(const TokenType &token) {
  return std::holds_alternative<UnaryFunction>(token);
}

bool Tokenizer::isOperatorButNotAParen(const Operator op) {
  return op != Operator::LParen and op != Operator::RParen;
}

bool Tokenizer::isAParen(const Operator op) {
  return op == Operator::LParen or op == Operator::RParen;
}
bool Tokenizer::isVariable(const TokenType &tok) {
  return std::holds_alternative<Variable>(tok);
}
bool Tokenizer::isFuncOperator(const Tokenizer::TokenType tok) {
  if (std::holds_alternative<Operator>(tok)) {
    auto op = std::get<Operator>(tok);
    switch (op) {

    case Operator::Sine:
    case Operator::Cosine:
    case Operator::Tan:
    case Operator::Exp:
    case Operator::Sqrt:
      return true;
    case Operator::None:
    default:
      return false;
    }
  } else {
    return false;
  }
}
auto Tokenizer::getFuncForOperator(const Tokenizer::Operator tok)
    -> UnaryFunction {
  switch (tok) {
  case Operator::Sine:
    return UnaryFunction{"sin", [](double x) { return std::sin(x); }};
  case Operator::Cosine:
    return UnaryFunction{"cos", [](double x) { return std::cos(x); }};
  case Operator::Tan:
    return UnaryFunction{"tan", [](double x) { return std::tan(x); }};
  case Operator::Exp:
    return UnaryFunction{"exp", [](double x) { return std::exp(x); }};
  case Operator::Sqrt:
    return UnaryFunction{"sqrt", [](double x) { return std::sqrt(x); }};
  default:
    throw std::runtime_error("Invalid operator");
  }
}
int Tokenizer::getOperatorPrecedence(const Operator &op) {
  if (op == Operator::LParen || op == Operator::RParen) {
    return -1;
  }
  return operator_info.at(op).precedence;
}
Tokenizer::Associativity
Tokenizer::getAssociativity(const Tokenizer::Operator &op) {
  using namespace Tokenizer;
  if (op == Operator::LParen || op == Operator::RParen) {
    return Associativity::None;
  }
  return operator_info.at(op).associativity;
}

// auto Tokenizer::tokenize(const std::string_view expression)
//     -> std::vector<TokenType> {
//   decltype(tokenize(expression)) vec{};
//   char curr = 0;
//   std::ostringstream oss{};
//   for (int pos = 0; pos < std::size(expression); ++pos) {
//     curr = expression.at(pos);
//     Logger::log(fmt::format("current char: expr[{}] = {} \n", pos, curr));
//     if (isdigit(curr) || curr == '.') {
//       oss << curr;
//     } else {
//       if (std::isalpha(curr)) {
//         if (oss.str().empty() && std::isalpha(expression.at(pos + 1))) {
//           const auto fn = expression.substr(pos, 3);
//           std::cout << fn << std::endl;

//           if (un_fns.find(fn) != std::end(un_fns)) {

//             // oss = {};
//             // pos += 4;
//             // fmt::println("[{} || {}]\n ", expression.substr(pos), pos);
//             // if (std::isalpha(expression.at(pos))) {
//             //   vec.emplace_back(un_fns.at(fn));
//             //   vec.emplace_back(Variable{expression.at(pos), 0.0});
//             //   pos += 2;
//             //   continue;
//             // } else {
//             //   auto fnn = un_fns.at(fn);
//             //   while (std::isdigit(expression.at(pos))) {
//             //     oss << expression.at(pos);
//             //     ++pos;
//             //   }
//             //   vec.emplace_back(fnn.fn(stod(oss.str())));
//             //   oss = {};
//             //   pos++;
//             //   continue;

//             // }

//             pos += 4;
//             while (expression.at(pos) != ')') {
//               oss << expression.at(pos);
//               ++pos;
//             }
//             fmt::println("{}", oss.str());
//             for (const auto &t : tokenize(oss.str())) {
//               vec.emplace_back(t);
//             }
//             ++pos;
//             continue;
//           }
//         } else if (oss.str().empty() && !isalpha(expression.at(pos + 1))) {
//           vec.emplace_back(Variable{curr, 0.0});
//         } else {
//           auto res = oss.str();
//           if (std::all_of(std::begin(res), std::end(res),
//                           [](char c) { return std::isdigit(c); })) {

//             const double res = std::stod(oss.str());
//             // check if the double is valid
//             if (not std::isnan(res)) {
//               // push the number, clear the buffer.
//               Logger::log(
//                   fmt::format("Token (number = {}) has been pushed", res),
//                   Logger::LogLevel::kInfo);
//               vec.emplace_back(res);
//               vec.emplace_back(Operator::Mult);
//               vec.emplace_back(Variable{curr, 0.0});

//               oss = {};
//               continue;
//             }
//           }
//         }
//       }

//       if (isOperator(static_cast<Operator>(curr)) and
//           static_cast<Operator>(curr) == Operator::Sub) {
//         if (oss.str().empty()) {
//           vec.emplace_back(-1.0);
//           vec.emplace_back(Operator::Mult);
//           oss = {};
//           continue;
//         }
//       }
//       // stopped getting digits in
//       if (!oss.str().empty()) {
//         // convert the digits acc'ed into a double
//         const double res = std::stod(oss.str());
//         // check if the double is valid
//         if (not std::isnan(res)) {
//           // push the number, clear the buffer.
//           Logger::log(fmt::format("Token (number = {}) has been pushed",
//           res),
//                       Logger::LogLevel::kInfo);
//           vec.emplace_back(res);
//           oss = {};
//         }
//       }
//       if (isOperator(static_cast<Operator>(curr))) {
//         Logger::log(fmt::format("Token (Operator = {}) has been pushed",
//         curr),
//                     Logger::LogLevel::kInfo);
//         vec.emplace_back(static_cast<Operator>(curr));
//         oss = {};
//       }
//     }
//   }
//   // Recheck for the last char.
//   if (!oss.str().empty()) {
//     // convert the digits acc'ed into a double
//     const double res = std::stod(oss.str());
//     // check if the double is valid
//     if (not std::isnan(res)) {
//       // push the number, clear the buffer.
//       Logger::log(fmt::format("Token (number = {}) has been pushed", res),
//                   Logger::LogLevel::kInfo);
//       vec.emplace_back(res);
//       oss = {};
//     }
//   }
//   fmt::println("{}", oss.str());
//   return vec;
// }
auto Tokenizer::tokenize(const std::string_view expression)
    -> std::vector<TokenType> {
  std::vector<TokenType> vec{};
  std::ostringstream oss{};

  for (size_t pos = 0; pos < expression.size(); ++pos) {
    char curr = expression[pos];

    if (isdigit(curr) || curr == '.') {
      oss << curr;
    } else if (std::isalpha(curr)) {
      std::string function_name;
      size_t function_start = pos;

      // Check if it's a function call
      while (pos < expression.size() && std::isalpha(expression[pos])) {
        function_name += expression[pos];
        ++pos;
      }

      if (pos < expression.size() && expression[pos] == '(') {
        // It's a function call
        Operator func_op;
        if (function_name == "sin")
          func_op = Operator::Sine;
        else if (function_name == "cos")
          func_op = Operator::Cosine;
        else if (function_name == "tan")
          func_op = Operator::Tan;
        else if (function_name == "exp")
          func_op = Operator::Exp;
        else if (function_name == "sqrt")
          func_op = Operator::Sqrt;
        else
          throw std::runtime_error("Unknown function: " + function_name);

        vec.emplace_back(func_op);
        vec.emplace_back(Operator::LParen);

        // Find matching closing parenthesis
        int paren_count = 1;
        size_t close_paren_pos = pos + 1;
        while (close_paren_pos < expression.size() && paren_count > 0) {
          if (expression[close_paren_pos] == '(')
            ++paren_count;
          if (expression[close_paren_pos] == ')')
            --paren_count;
          ++close_paren_pos;
        }

        if (paren_count != 0) {
          throw std::runtime_error("Mismatched parentheses in function call");
        }

        // Recursively tokenize function arguments
        auto inner_tokens =
            tokenize(expression.substr(pos + 1, close_paren_pos - pos - 2));
        vec.insert(vec.end(), inner_tokens.begin(), inner_tokens.end());

        vec.emplace_back(Operator::RParen);
        pos = close_paren_pos - 1;
      } else {
        // It's a variable
        vec.emplace_back(Variable{function_name[0], 0.0});
        pos = function_start;
      }
    } else if (isOperator(static_cast<Operator>(curr))) {
      if (!oss.str().empty()) {
        vec.emplace_back(std::stod(oss.str()));
        oss.str("");
      }
      vec.emplace_back(static_cast<Operator>(curr));
    } else if (!std::isspace(curr)) {
      throw std::runtime_error(std::string("Unexpected character: ") + curr);
    }
  }

  if (!oss.str().empty()) {
    vec.emplace_back(std::stod(oss.str()));
  }

  return vec;
}
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
      : m_idx(idx), m_expr(expr),
        m_what(fmt::format(
            "Invalid Expression. Missing Parenthesis {}\n {}\n{}", m_idx,
            m_expr, getCaretToMatchErrorPosition(m_idx, to_match))) {}

  virtual const char *what() const noexcept override { return m_what.c_str(); }
};

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

std::vector<Tokenizer::TokenType>
Tokenizer::shunting_yard(const std::string &expression) {
  using namespace Logger;
  using Logger::LogLevel;

  std::vector<TokenType> tokens = tokenize(expression);
  std::vector<TokenType> output_queue{};
  std::stack<Operator> op_stack{};
  std::ostringstream oss{};
  for (const auto &token : tokens) {
    if (isNumber(token)) {
      output_queue.emplace_back(token);
      oss << std::get<double>(token);
    } else if (isVariable(token)) {
      output_queue.emplace_back(token);
      oss << std::get<Variable>(token);
    } else if (isFunction(token)) {
      auto fn = std::get<UnaryFunction>(token);
      if (fn.name == "sin") {
        op_stack.emplace(Operator::Sine);
      } else if (fn.name == "cos") {
        op_stack.emplace(Operator::Cosine);
      } else if (fn.name == "tan") {
        op_stack.emplace(Operator::Tan);
      } else if (fn.name == "sqrt") {
        op_stack.emplace(Operator::Sqrt);
      } else if (fn.name == "exp") {
        op_stack.emplace(Operator::Exp);
      }
      continue;
    } else if (isOperator(token)) {
      auto op = std::get<Operator>(token);
      if (isOperatorButNotAParen(op)) {
        auto o1 = std::get<Operator>(token);
        Logger::log(fmt::format("Token (Operator = {}) is found.",
                                static_cast<char>(o1)));
        while ((not op_stack.empty() && op_stack.top() != Operator::LParen) and
               ((getOperatorPrecedence(op_stack.top()) >
                 getOperatorPrecedence(o1)) or
                ((getOperatorPrecedence(op_stack.top()) ==
                  getOperatorPrecedence(o1)) and
                 getAssociativity(o1) == Associativity::Left))) {
          output_queue.emplace_back(op_stack.top());
          oss << static_cast<char>(op_stack.top());
          op_stack.pop();
        }
        op_stack.push(o1);
      } else if (isAParen(op)) {
        if (op == Operator::LParen) {
          op_stack.emplace(op);
          continue;
        } else if (op == Operator::RParen) {
          while (not op_stack.empty() and op_stack.top() != Operator::LParen) {
            output_queue.emplace_back(op_stack.top());
            oss << static_cast<char>(op_stack.top());
            op_stack.pop();
          }
          assert(op_stack.top() == Operator::LParen);
          op_stack.pop();
        }
      }
    }
  }

  while (not op_stack.empty()) {

    output_queue.emplace_back(op_stack.top());
    oss << op_stack.top();
    op_stack.pop();
  }
  Logger::log(fmt::format("Output Queue: {}", oss.str()), LogLevel::kInfo);
  return output_queue;
}
