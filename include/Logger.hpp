// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file include/Logger.hpp
 * @brief This file defines the Logger class for logging messages
 *  with different log levels to the serial port.
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <Arduino.h>
#include <Constants.hpp>
#include <string>

enum LogLevel
{
	DEBUG,
	INFO,
	WARN,
	ERROR
};

class Logger
{
public:
	static void log(LogLevel level, const std::string& message);
	static void log(LogLevel level, const char* message);
	static void log(LogLevel level, const String& message);

private:
	static void printPrefix(LogLevel level);
};

#endif // LOGGER_HPP