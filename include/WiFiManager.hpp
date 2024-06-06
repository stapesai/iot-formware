// Path: include/WiFiManager.hpp

#ifndef WIFI_MANAGER_HPP
#define WIFI_MANAGER_HPP

#include <WiFi.h>
#include <Arduino.h>
#include <NVSManager.hpp>

class WiFiManager
{
public:
	WiFiManager(NVSManager* config);
	void initialiseWiFi();
	void connectWiFi();
	void printWiFiStatus();

private:
	NVSManager* config;
	void setupWiFi();
};

#endif // WIFI_MANAGER_HPP
