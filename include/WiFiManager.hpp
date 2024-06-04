// Path: include/WiFiManager.hpp

#ifndef WIFI_MANAGER_HPP
#define WIFI_MANAGER_HPP

#include "NVSManager.hpp"
#include <Arduino.h>
#include <WiFi.h>

class WiFiManager {
  public:
    WiFiManager(NVSManager *config);
    void initialiseWiFi();
    void connectWiFi();
    void printWiFiStatus();

  private:
    NVSManager *config;
    void setupWiFi();
};

#endif // WIFI_MANAGER_HPP
