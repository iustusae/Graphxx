#pragma once
#define FNP_CST_NOEX const noexcept
#include <fmt/core.h>

#include <cassert>
#include <cmath>
#include <cstdio>
#include <functional>
#include <iostream>
#include <variant>

namespace types {
class TreeNode {
 public:
  virtual void print() FNP_CST_NOEX = 0;
  virtual double eval() FNP_CST_NOEX = 0;
  virtual ~TreeNode() = default;  // Ensure proper cleanup
};

struct InfixOperator : public TreeNode {
  TreeNode *left{};
  TreeNode *right{};
};

class PrefixUnaryOperator : public TreeNode {
 public:
  TreeNode *arg;
};

class ValueNode : public TreeNode {
 public:
  double value{};
  ValueNode(double value) : value(value) {}
  virtual void print() FNP_CST_NOEX override { fmt::print("{}", value); }
  virtual double eval() FNP_CST_NOEX override { return value; }
};

struct BinaryOperation_t {
  char sign{};
  std::function<double(double, double)> operation{};
};

class BinaryOperator : public InfixOperator {
 protected:
  BinaryOperation_t p_operation{};
  BinaryOperator(const BinaryOperation_t &bin) : p_operation(bin) {}

 public:
  virtual void print() FNP_CST_NOEX override {
    if (!left || !right) {
      fmt::print("null.");
      return;
    }
    fmt::print("({} {} {})", left->eval(), p_operation.sign, right->eval());
  }

  virtual double eval() FNP_CST_NOEX override {
    if (!left || !right) return NAN;
    return p_operation.operation(left->eval(), right->eval());
  }
};

class Add : public BinaryOperator {
 public:
  Add() : BinaryOperator({'+', [&](double a, double b) { return a + b; }}) {}
};

class Subtract : public BinaryOperator {
 public:
  Subtract()
      : BinaryOperator({'-', [&](double a, double b) { return a - b; }}) {}
};
class Divide : public BinaryOperator {
 public:
  Divide()
      : BinaryOperator({'/', [&](double a, double b) {
                          assert(b != 0);
                          return a / b;
                        }}) {}
};
class Multiply : public BinaryOperator {
 public:
  Multiply()
      : BinaryOperator({'*', [&](double a, double b) { return a * b; }}) {}
};

class Negate : public TreeNode {
 public:
  ValueNode *node{};
  virtual void print() FNP_CST_NOEX override { fmt::print("{}", node->eval()); }
  virtual double eval() FNP_CST_NOEX override { return -(node->eval()); }

  ~Negate() { delete node; }
};

};  // namespace types
