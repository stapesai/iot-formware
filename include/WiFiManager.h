// Path: src/WiFiManager.h

#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <NVSManager.h>

class WiFiManager
{
public:
    WiFiManager(NVSManager *config);
    void initializeWiFi();

private:
    NVSManager *config;
};

#endif // WIFIMANAGER_H
