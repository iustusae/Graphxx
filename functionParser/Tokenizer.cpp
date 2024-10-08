#include "Tokenizer.hpp"

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
bool Tokenizer::isFunction(const TokenType &token) { return false; }

bool Tokenizer::isOperatorButNotAParen(const Operator op) {
  return op != Operator::LParen and op != Operator::RParen;
}

bool Tokenizer::isAParen(const Operator op) {
  return op == Operator::LParen or op == Operator::RParen;
}
bool Tokenizer::isVariable(const TokenType &tok) {
  return std::holds_alternative<Variable>(tok);
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

auto Tokenizer::tokenize(const std::string_view expression)
    -> std::vector<TokenType> {
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

  return vec;
}

inline std::vector<Tokenizer::TokenType>
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
  return output_queue;
}

double Tokenizer::evaluate(const std::string &expression) {
  auto vec = shunting_yard(expression);
  std::stack<double> accumulator{};

  for (const auto &tok : vec) {
    if (isNumber(tok)) {
      accumulator.emplace(std::get<double>(tok));
    } else if (isOperator(tok)) {
      auto op = std::get<Operator>(tok);

      double lhs{}, rhs{};
      switch (op) {
      case Operator::Sum:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        accumulator.emplace(lhs + rhs);
        break;
      case Operator::Sub:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        accumulator.emplace(lhs - rhs);
        break;
      case Operator::Div:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        assert(rhs != 0);
        accumulator.emplace(lhs / rhs);
        break;
      case Operator::Mult:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        accumulator.emplace(lhs * rhs);
        break;
      case Operator::Pow:
        lhs = {accumulator.top()};
        accumulator.pop();
        rhs = {accumulator.top()};
        accumulator.pop();
        accumulator.emplace(op_to_fn.at(Operator::Pow).fn(lhs, rhs));
        break;
      default:
        return 0;
      }
    }
  }
  return accumulator.top();
}