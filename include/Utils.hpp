// Path: include/Utils.hpp

#ifndef UTILS_H
#define UTILS_H


#include <sstream>
#include <WiFi.h>
#include <Logger.hpp>
#include <NVSManager.hpp>

class Utils
{
public:
	std::vector<std::string> formatString(const std::string& str, char delim);
	std::string generateName();
	static void statusBuzzer(int times, int delayTime);
	void printConfig(const NVSManager& config);
	void restartDevice(unsigned long delayTime);
};

#endif // UTILS_H
