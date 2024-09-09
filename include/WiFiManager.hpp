// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file include/WiFiManager.hpp
 * @brief This file contains the declaration of the WiFiManager class, 
 *  which handles connecting the ESP32 to a WiFi network.
 */

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