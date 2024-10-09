#pragma once
#include "Logger.hpp"
#include <fmt/core.h>

#include <cassert>
#include <cctype>
#include <cmath>
#include <functional>
#include <iostream>
#include <queue>
#include <sstream>
#include <stack>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

/**
 * @namespace Tokenizer
 * @brief A namespace for parsing and evaluating mathematical expressions.
 *
 * This namespace provides functionality for tokenizing expressions,
 * transforming them to Reverse Polish Notation (RPN) using the Shunting Yard
 * algorithm, and evaluating mathematical expressions. It includes structures
 * for operators, functions, variables, and utilities for handling tokens.
 */
namespace Tokenizer {

/**
 * @enum Operator
 * @brief Enum class representing various mathematical operators.
 *
 * This enum defines the character representations for different operators used
 * in mathematical expressions.
 */
enum class Operator : char {
  Sum = '+',    ///< Addition operator
  Sub = '-',    ///< Subtraction operator
  Div = '/',    ///< Division operator
  Pow = '^',    ///< Exponentiation operator
  Mult = '*',   ///< Multiplication operator
  LParen = '(', ///< Left parenthesis
  RParen = ')', ///< Right parenthesis
  Comma = ',',  ///< Comma (for separating function arguments)
  Sine = 's',   ///< Sine function
  Cosine = 'c', ///< Cosine function
  Tan = 't',    ///< Tangent function
  Exp = 'e',    ///< Exponential function
  Sqrt = 'r',
  None = '\0' ///< No operator
};

/**
 * @struct UnaryFunction
 * @brief Represents a unary mathematical function.
 *
 * This structure captures the name of the function and a callable that
 * performs the operation on a single double value.
 */
struct UnaryFunction {
  std::string name{};                 ///< Name of the unary function
  std::function<double(double)> fn{}; ///< Function to apply
};

/**
 * @struct BinaryFunction
 * @brief Represents a binary mathematical function.
 *
 * This structure includes the name of the function and a callable that
 * performs the operation on two double values.
 */
struct BinaryFunction {
  std::string name;                           ///< Name of the binary function
  std::function<double(double, double)> fn{}; ///< Function to apply
};

/**
 * @enum Associativity
 * @brief Enum class representing the associativity of operators.
 */
enum class Associativity {
  Left,  ///< Left associative
  Right, ///< Right associative
  None,  ///< No associativity
};

/**
 * @struct OperatorInfo
 * @brief Contains information about an operator.
 *
 * This structure holds the precedence and associativity of a given operator.
 */
struct OperatorInfo {
  int precedence{};              ///< Precedence level of the operator
  Associativity associativity{}; ///< Associativity of the operator
};

/**
 * @struct Variable
 * @brief Represents a variable in an expression.
 *
 * This structure captures the name and value of a variable.
 */
struct Variable {
  char name{};    ///< Name of the variable
  double value{}; ///< Value of the variable
};

/**
 * @typedef TokenType
 * @brief A variant type representing different token types.
 *
 * This type can hold a double, a variable, an operator, a unary function, or a
 * binary function.
 */
using TokenType =
    std::variant<double, Variable, Operator, UnaryFunction, BinaryFunction>;

/**
 * @brief Checks if the given character is a valid operator.
 * @param c The character to check.
 * @return true if the character is an operator, false otherwise.
 */
bool isOperator(const Operator c);

/**
 * @brief Overloads the output stream operator for Operator.
 * @param out The output stream.
 * @param op The operator to output.
 * @return The modified output stream.
 */
std::ostream &operator<<(std::ostream &out, const Operator &op);

/**
 * @brief Overloads the output stream operator for UnaryFunction.
 * @param out The output stream.
 * @param func The unary function to output.
 * @return The modified output stream.
 */
std::ostream &operator<<(std::ostream &out, const UnaryFunction &func);

/**
 * @brief Overloads the output stream operator for BinaryFunction.
 * @param out The output stream.
 * @param func The binary function to output.
 * @return The modified output stream.
 */
std::ostream &operator<<(std::ostream &out, const BinaryFunction &func);

/**
 * @brief Overloads the output stream operator for Variable.
 * @param out The output stream.
 * @param var The variable to output.
 * @return The modified output stream.
 */
std::ostream &operator<<(std::ostream &out, const Variable &var);

/**
 * @brief Overloads the output stream operator for a queue of TokenType.
 * @param out The output stream.
 * @param q The queue of tokens to output.
 * @return The modified output stream.
 */
std::ostream &operator<<(std::ostream &out, const std::queue<TokenType> &q);

/**
 * @brief Maps operators to their corresponding binary functions.
 */
const inline std::unordered_map<Operator, BinaryFunction> op_to_fn = {
    {Operator::Sum,
     BinaryFunction{"Sum", [](double a, double b) { return a + b; }}},
    {Operator::Sub,
     BinaryFunction{"Sub", [](double a, double b) { return a - b; }}},
    {Operator::Div, BinaryFunction{"Div",
                                   [](double a, double b) {
                                     assert(b != 0);
                                     return a / b;
                                   }}},
    {Operator::Mult,
     BinaryFunction{"Mult", [](double a, double b) { return a * b; }}},
    {Operator::Pow,
     BinaryFunction{"Pow", [](double a, double b) { return std::pow(a, b); }}},
};

/**
 * @brief Maps operators to their precedence and associativity information.
 */
const inline std::unordered_map<Operator, OperatorInfo> operator_info = {
    {Operator::Pow, {4, Associativity::Right}},
    {Operator::Mult, {3, Associativity::Left}},
    {Operator::Div, {3, Associativity::Left}},
    {Operator::Sub, {2, Associativity::Left}},
    {Operator::Sum, {2, Associativity::Left}},
};

const inline std::unordered_map<std::string_view, UnaryFunction> un_fns{
    {"sin", UnaryFunction{"sin", [](double a) { return std::sin(a); }}},
    {"cos", UnaryFunction{"cos", [](double a) { return std::cos(a); }}},
    {"tan", UnaryFunction{"tan", [](double a) { return std::tan(a); }}},
    {"sqrt", UnaryFunction{"sqrt", [](double a) { return std::sqrt(a); }}},
    {"log", UnaryFunction{"log", [](double a) { return std::log(a); }}},
    {"exp", UnaryFunction{"exp", [](double a) { return std::exp(a); }}},

};

/**
 * @brief Prints a token to the console.
 * @param token The token to print.
 */
inline void print_token(const TokenType &token);

/**
 * @brief Prints a container of tokens to the console.
 * @param cont The vector of tokens to print.
 */
inline void print_container(const std::vector<TokenType> &cont);

/**
 * @brief Checks if the token is a number.
 * @param token The token to check.
 * @return true if the token is a number, false otherwise.
 */
bool isNumber(const TokenType &token);

/**
 * @brief Checks if the token is an operator.
 * @param token The token to check.
 * @return true if the token is an operator, false otherwise.
 */
bool isOperator(const TokenType &token);

/**
 * @brief Checks if the token represents a function.
 * @param token The token to check.
 * @return true if the token is a function, false otherwise.
 */
bool isFunction(const TokenType &token);

/**
 * @brief Checks if the operator is not a parenthesis.
 * @param op The operator to check.
 * @return true if the operator is not a parenthesis, false otherwise.
 */
bool isOperatorButNotAParen(const Operator op);

/**
 * @brief Checks if the token is a variable.
 * @param tok The token to check.
 * @return true if the token is a variable, false otherwise.
 */
bool isVariable(const TokenType &tok);

/**
 * @brief Checks if the operator is a parenthesis.
 * @param op The operator to check.
 * @return true if the operator is a parenthesis, false otherwise.
 */
bool isAParen(const Operator op);

bool isFuncOperator(const Tokenizer::TokenType tok);
auto getFuncForOperator(const Tokenizer::Operator tok) -> UnaryFunction;
/**
 * @brief Gets the precedence of an operator.
 * @param op The operator whose precedence is to be determined.
 * @return The precedence level of the operator.
 */
int getOperatorPrecedence(const Operator &op);

/**
 * @brief Gets the associativity of an operator.
 * @param op The operator whose associativity is to be determined.
 * @return The associativity of the operator.
 */
Associativity getAssociativity(const Operator &op);

/**
 * @brief Tokenize the given mathematical expression.
 * @param expression The expression to tokenize.
 * @return A vector of TokenType representing the tokenized expression.
 */
auto tokenize(const std::string_view expression) -> std::vector<TokenType>;

/**
 * @brief Converts a TokenType to a string representation.
 * @param token The token to convert.
 * @return A string representation of the token.
 */
inline std::string tokenToString(const TokenType &token);

/**
 * @brief Implements the Shunting Yard algorithm to convert infix expressions
 * to RPN.
 *
 * This algorithm was adapted from the Wikipedia page:
 * https://en.wikipedia.org/wiki/Shunting_yard_algorithm
 *
 * @param expression The infix expression to convert.
 * @return A vector of TokenType representing the expression in RPN.
 */
std::vector<TokenType> shunting_yard(const std::string &expression);

/**
 * @brief Evaluates a mathematical expression given as a string.
 * @param expression The expression to evaluate.
 * @return The result of the evaluation as a double.
 */

template <class Output>
Output evaluate(const std::string &expression,
                const std::unordered_map<char, double> &var_values = {}) {

  static_assert(std::is_arithmetic<Output>::value, "Output must be arithmetic");

  auto vec = shunting_yard(expression);
  if (var_values.empty()) {
    for (auto &token : vec) {
      if (isVariable(token)) {
        std::get<Variable>(token).value = 0;
      }
    }
  } else {

    for (auto &token : vec) {
      if (isVariable(token)) {
        if (var_values.find(std::get<Variable>(token).name) ==
            var_values.end()) {
          std::get<Variable>(token).value = 0;
        } else {
          std::get<Variable>(token).value =
              var_values.at(std::get<Variable>(token).name);
        }
      }
    }
  }
  std::stack<double> accumulator{};

  for (const auto &tok : vec) {
    if (isNumber(tok)) {
      accumulator.emplace(std::get<double>(tok));
    } else if (isVariable(tok)) {
      accumulator.emplace(std::get<Variable>(tok).value);
    } else if (isFuncOperator(tok)) {
      auto op = std::get<Operator>(tok);
      double argument = accumulator.top();
      accumulator.pop();
      auto fn = getFuncForOperator(op);
      accumulator.emplace(fn.fn(argument));
    } else if (isOperator(tok)) {
      auto op = std::get<Operator>(tok);

      double lhs{}, rhs{};
      switch (op) {
      case Operator::Sum:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        Logger::log(
            fmt::format("Performing operation {} on {} and {}", '+', lhs, rhs));
        accumulator.emplace(rhs + lhs);
        break;
      case Operator::Sub:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        Logger::log(
            fmt::format("Performing operation {} on {} and {}", '-', lhs, rhs));
        accumulator.emplace(rhs - lhs);
        break;
      case Operator::Div:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        Logger::log(
            fmt::format("Performing operation {} on {} and {}", '/', lhs, rhs));
        accumulator.emplace(rhs / lhs);
        break;
      case Operator::Mult:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        Logger::log(
            fmt::format("Performing operation {} on {} and {}", '*', lhs, rhs));
        accumulator.emplace(rhs * lhs);
        break;
      case Operator::Pow:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        accumulator.emplace(std::pow(rhs, lhs));
        break;
      default:
        return 0;
      }

    } else {
    }
  }

  return (std::is_integral<Output>::value)
             ? static_cast<Output>(std::ceil(accumulator.top()))
             : static_cast<Output>(accumulator.top());
}

std::vector<std::pair<double, double>> getAllPoints(int max_y);

} // namespace Tokenizer
