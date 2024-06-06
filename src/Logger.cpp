// Path: src/Logger.cpp

#include "Logger.h"

void Logger::log(LogLevel level, const std::string &message) {
  printPrefix(level);
  Serial.println(message.c_str());
}

void Logger::log(LogLevel level, const char *message) {
  printPrefix(level);
  Serial.println(message);
}

void Logger::log(LogLevel level, const String &message) {
  printPrefix(level);
  Serial.println(message);
}

void Logger::printPrefix(LogLevel level) {
  switch (level) {
  case DEBUG:
    Serial.print("[DEBUG] ");
    break;
  case INFO:
    Serial.print("[INFO] ");
    break;
  case WARN:
    Serial.print("[WARN] ");
    break;
  case ERROR:
    Serial.print("[ERROR] ");
    break;
  }
}
