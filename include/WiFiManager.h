#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "NVSManager.h"
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

#endif // WIFI_MANAGER_H
