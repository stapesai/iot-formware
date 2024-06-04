// Path: include/Utils.hpp

#ifndef UTILS_H
#define UTILS_H

#include "NVSManager.hpp"
#include <WiFi.h>
#include <iostream>
#include <sstream>

class Utils {
  public:
    std::vector<std::string> formatString(const std::string &str, char delim);
    static void statusBuzzer(int times, int delayTime);
    void printConfig(const NVSManager &config);
    void restartDevice(unsigned long delayTime);
    const char *generateName();
};

#endif // UTILS_H
