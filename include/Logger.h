// Path: include/Logger.h

#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <string>

enum LogLevel { DEBUG, INFO, WARN, ERROR };

class Logger {
public:
  static void log(LogLevel level, const std::string &message);
  static void log(LogLevel level, const char *message);
  static void log(LogLevel level, const String &message);

private:
  static void printPrefix(LogLevel level);
};

#endif // LOGGER_H