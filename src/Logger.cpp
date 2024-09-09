// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file src/Logger.cpp
 * @brief This file contains the implementation of the Logger class.
 */

#include <Logger.hpp>

String colorCode;
String bgColorCode;

void Logger::log(LogLevel level, const std::string& message)
{
	printPrefix(level);
	Serial.print(" ");
	Serial.println(message.c_str());
}

void Logger::log(LogLevel level, const char* message)
{
	printPrefix(level);
	Serial.print(" ");
	Serial.println(message);
}

void Logger::log(LogLevel level, const String& message)
{
	printPrefix(level);
	Serial.print(" ");
	Serial.println(message);
}

void Logger::printPrefix(LogLevel level)
{
	String logLevelStr;
	String colorCode;
	String bgColorCode;

	switch(level)
	{
	case INFO:
		logLevelStr = " [INFO]  ";
		bgColorCode = BG_BLUE;
		colorCode = RED;
		break;
	case WARN:
		logLevelStr = " [WARN]  ";
		bgColorCode = BG_ORANGE;
		colorCode = WHITE;
		break;
	case ERROR:
		logLevelStr = " [ERROR] ";
		bgColorCode = BG_RED;
		colorCode = WHITE;
		break;
	case DEBUG:
		logLevelStr = " [DEBUG] ";
		bgColorCode = BG_GREEN;
		colorCode = WHITE;
		break;
	}

	Serial.print(bgColorCode); // Background color box
	Serial.print(colorCode);
	Serial.print(logLevelStr);
	Serial.print(RESET_COLOR); // Reset color after log level
}
