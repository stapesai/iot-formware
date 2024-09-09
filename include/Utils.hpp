// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file include/Utils.hpp
 * @brief  This file contains the declaration of various utility functions
 *  used throughout the project.
 */
#ifndef UTILS_H
#define UTILS_H

#include <Logger.hpp>
#include <NVSManager.hpp>
#include <WiFi.h>

#include <iostream>
#include <map>
#include <sstream>

class Utils
{
public:
	static std::string getMacAddr();
	static void statusBuzzer(int times, int delayTime);
	static void printConfig(const NVSManager& config);
	static void restartDevice(unsigned long delayTime);
	static std::map<std::string, std::string> convertToKVMap(const std::string& str);
	static std::string convertToKVString(const std::map<std::string, std::string>& kvMap);
	static std::vector<std::string> splitString(std::string string, char delim);
};

#endif // UTILS_H