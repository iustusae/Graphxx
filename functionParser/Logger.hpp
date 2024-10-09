#pragma once
#include <string>
namespace Logger {

enum class LogLevel {
  kAll,
  kInfo,
  kWarn,
  kError,
  kSevere,
  kMax,
};

inline void log(const std::string &str,
                Logger::LogLevel level = LogLevel::kAll);

#ifdef FNP_DBG
#include <cstdint>
#include <iostream>
#include <termcolor/termcolor.hpp>
inline void log(const std::string &str, Logger::LogLevel level) {
  switch (level) {
  case LogLevel::kInfo:
    std::clog << termcolor::bright_blue << termcolor::italic << str << '\n';
    break;
  case LogLevel::kWarn:
    std::clog << termcolor::bright_red << termcolor::italic << str << '\n';
    break;
  case LogLevel::kError:
    std::clog << termcolor::red << termcolor::bold << str << '\n';
    break;
  case LogLevel::kSevere:
    std::clog << termcolor::dark << termcolor::blink << str << '\n';
    break;
  default:
    std::clog << termcolor::white << str << '\n';
    break;
  }
}
#else
inline void log(const std::string &str, Logger::LogLevel level) { return; }
#endif

} // namespace Logger