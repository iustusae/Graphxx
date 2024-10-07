#pragma once
#include <fmt/core.h>

#include "Logger.hpp"
#include <cassert>
#include <cctype>
#include <cmath>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>
#include <string_view>
#include <termcolor/termcolor.hpp>
#include <variant>
#include <vector>
#include <stack>

namespace Tokenizer {
enum class Operator : char {
  Sum = '+',
  Sub = '-',
  Div = '/',
  Pow = '^',
  Mult = '*',
  LParen = '(',
  RParen = ')',
  Comma = ',',
  None = '\0'
};
bool isOperator(const Operator c) {
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

std::ostream& operator<<(std::ostream& out, const Operator& op) {
  out << static_cast<char>(op);
  return out;
}

struct UnaryFunction {
  std::string name{};
  std::function<double(double)> fn{};
};
std::ostream& operator<<(std::ostream& out, const UnaryFunction& func) {
  out << func.name;
  return out;
}
struct BinaryFunction {
  std::string name;
  std::function<double(double, double)> fn{};
};

std::unordered_map<Operator, BinaryFunction> op_to_fn = {
    {Operator::Sum,
     BinaryFunction{"Sum", [](double a, double b) { return a + b; }}},
    {Operator::Sub,
     BinaryFunction{"Sum", [](double a, double b) { return a - b; }}},
    {Operator::Div, BinaryFunction{"Sum",
                                   [](double a, double b) {
                                     assert(b != 0);
                                     return a / b;
                                   }}},
    {Operator::Mult,
     BinaryFunction{"Sum", [](double a, double b) { return a * b; }}},
    {Operator::Pow,
     BinaryFunction{"Sum", [](double a, double b) { return std::pow(a, b); }}},

};

enum class Associativity {
  Left,
  Right,
  None,
};
struct OperatorInfo {
  int precedence{};
  Associativity associativity{};
};
std::unordered_map<Operator, OperatorInfo> operator_info{
    {Operator::Pow, {4, Associativity::Right}},
    {Operator::Mult, {3, Associativity::Left}},
    {Operator::Div, {3, Associativity::Left}},
    {Operator::Sub, {2, Associativity::Left}},
    {Operator::Sum, {2, Associativity::Left}},
};

std::ostream& operator<<(std::ostream& out, const BinaryFunction& func) {
  out << func.name;
  return out;
}

struct Variable {
  char name{};
  double value{};
};
std::ostream& operator<<(std::ostream& out, const Variable& var) {
  out << fmt::format("[{} = {}]", var.name, var.value);

  return out;
}

using TokenType =
    std::variant<double, Variable, Operator, UnaryFunction, BinaryFunction>;

void print_token(const TokenType& token) {
  std::visit(
      [](const auto& value) {
        std::cout << value << ", ";  // Print the value
      },
      token);
}

void print_container(const std::vector<TokenType>& cont) {
  for (const auto& item : cont) {
    print_token(item);
  }
  std::cout << std::endl;
}
auto tokenize(const std::string_view expression) -> std::vector<TokenType> {
  std::cout << expression << '\n';
  decltype(tokenize(expression)) vec{};
  char curr = 0;
  std::ostringstream oss{};
  for (int pos = 0; pos < std::size(expression); ++pos) {
    curr = expression.at(pos);
    Logger::log(fmt::format("current char: expr[{}] = {} \n", pos, curr));
    if (isdigit(curr) || curr == '.') {
      oss << curr;
    } else {
      // stopped getting digits in
      if (!oss.str().empty()) {
        // convert the digits acc'ed into a double
        const double res = std::stod(oss.str());
        // check if the double is valid
        if (not std::isnan(res)) {
          // push the number, clear the buffer.
          Logger::log(fmt::format("Token (number = {}) has been pushed", res),
                      Logger::LogLevel::kInfo);
          vec.emplace_back(res);
          oss = {};
        }
      }
      if (isOperator(static_cast<Operator>(curr))) {
        Logger::log(fmt::format("Token (Operator = {}) has been pushed", curr),
                    Logger::LogLevel::kInfo);
        vec.emplace_back(static_cast<Operator>(curr));
        oss = {};
      }
    }
  }
  // Recheck for the last char.
  if (!oss.str().empty()) {
    // convert the digits acc'ed into a double
    const double res = std::stod(oss.str());
    // check if the double is valid
    if (not std::isnan(res)) {
      // push the number, clear the buffer.
      Logger::log(fmt::format("Token (number = {}) has been pushed", res),
                  Logger::LogLevel::kInfo);
      vec.emplace_back(res);
      oss = {};
    }
  }

  print_container(vec);
  return vec;
}

bool isNumber(const TokenType& token) {
  return std::holds_alternative<double>(token);
}
bool isOperator(const TokenType& token) {
  return std::holds_alternative<Operator>(token);
}
bool isFunction(const TokenType& token) { return false; }

std::ostream& operator<<(std::ostream& out, const std::queue<TokenType>& q) {
  auto cp = q;

  while (not cp.empty()) {
    print_token(cp.front());
    cp.pop();
  }

  return out;
}

// Helper function to convert a TokenType to a string
inline std::string tokenToString(const TokenType& token) {
  if (std::holds_alternative<double>(token)) {
    return std::to_string(std::get<double>(token));
  } else if (std::holds_alternative<Operator>(token)) {
    return {static_cast<char>(std::get<Operator>(token))};
  }
  return "";
}

// Function to convert the entire queue to a string
inline std::string queueToString(const std::queue<TokenType>& queue) {
  std::ostringstream oss;
  std::queue<TokenType> temp_queue =
      queue;  // Create a copy to preserve the original queue

  while (!temp_queue.empty()) {
    oss << tokenToString(temp_queue.front());
    temp_queue.pop();
    if (!temp_queue.empty()) {
      oss << " ";  // Add space between tokens
    }
  }

  return oss.str();
}
bool isOperatorButNotAParen(const Operator op) {
  return op != Operator::LParen and op != Operator::RParen;
}
bool isAParen(const Operator op) {
  return op == Operator::LParen or op == Operator::RParen;
}

inline int getOperatorPrecedence(const Operator& op) {
  if (op == Operator::LParen || op == Operator::RParen) {
    return -1;
  }
  return operator_info[op].precedence;
}
inline Associativity getAssociativity(const Operator& op) {
  if (op == Operator::LParen || op == Operator::RParen) {
    return Associativity::None;
  }
  return operator_info[op].associativity;
}

/**
 * @brief Shunting yard algo to transform a classic math expression into RPN. Taken from https://en.wikipedia.org/wiki/Shunting_yard_algorithm
 * @anchor https://en.wikipedia.org/wiki/Shunting_yard_algorithm
 */
inline std::vector<TokenType> shunting_yard(const std::string& expression) {
  using namespace Logger;
  using Logger::LogLevel;

  std::vector<TokenType> tokens = tokenize(expression);
  std::vector<TokenType> output_queue{};
  std::stack<Operator> op_stack{};
  std::ostringstream oss{};
  for (const auto& token : tokens) {
    if (isNumber(token)) {
      output_queue.emplace_back(token);
      oss << std::get<double>(token);
    } else if (isOperator(token)) {
      auto op = std::get<Operator>(token);
      if(isOperatorButNotAParen(op)) {
      auto o1 = std::get<Operator>(token);
      Logger::log(fmt::format("Token (Operator = {}) is found.", static_cast<char>(o1)));
      while ((not op_stack.empty() &&
              op_stack.top() != Operator::LParen) and
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
        continue;
      } else if (isAParen(op)) {
        if(op == Operator::LParen) {
          op_stack.emplace(op);
          continue;
        } else if (op == Operator::RParen) {
            while(not op_stack.empty() and op_stack.top() != Operator::LParen) {
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

    while(not op_stack.empty()) {

        output_queue.emplace_back(op_stack.top());
        oss << op_stack.top();
        op_stack.pop();
    }
  return output_queue;
};  // namespace Tokenizer
}  // namespace Tokenizer