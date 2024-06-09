// Path: include/Utils.hpp

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
	static std::string convertToKVString(const std::map<std::string, std::string>& kv_map);
	static std::vector<std::string> splitString(std::string string, char delim);
};

#endif // UTILS_H
