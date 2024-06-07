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
	std::string generateName();
	// function to generate the mqtt topic
	std::string generateTopic(std::string& user_id, std::string& home_id, std::string& client_id);
	static void statusBuzzer(int times, int delayTime);
	static void printConfig(const NVSManager& config);
	static void restartDevice(unsigned long delayTime);
	static std::map<std::string, std::string> formatString(const std::string& str);
	static std::string convertToKVString(const std::map<std::string, std::string>& kv_map);
};

#endif // UTILS_H
