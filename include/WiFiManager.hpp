// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: include/WiFiManager.hpp

#ifndef WIFI_MANAGER_HPP
#define WIFI_MANAGER_HPP

#include <Arduino.h>
#include <NVSManager.hpp>
#include <WiFi.h>

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
