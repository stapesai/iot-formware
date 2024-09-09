// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file src/WiFiManager.cpp
 * @brief This file contains the implementation of the WiFiManager class.
 */
#include <Logger.hpp>
#include <WiFiManager.hpp>

WiFiManager::WiFiManager(NVSManager* config)
	: config(config)
{ }

void WiFiManager::initialiseWiFi()
{
	Logger::log(INFO, YELLOW "Initializing WiFi..." RESET_COLOR);
	setupWiFi();
	connectWiFi();
	printWiFiStatus();
}

void WiFiManager::setupWiFi()
{
	WiFi.mode(WIFI_STA);
	Logger::log(INFO, YELLOW "Setting up WiFi with SSID: " RESET_COLOR + config->getWiFiSSID());
	Logger::log(INFO, YELLOW "With password: " RESET_COLOR + config->getWiFiPassword());
}

void WiFiManager::connectWiFi()
{
	Logger::log(INFO, YELLOW "Connecting to WiFi..." RESET_COLOR);

	WiFi.begin(config->getWiFiSSID().c_str(), config->getWiFiPassword().c_str());

	uint8_t attemptCount = 0;
	while(WiFi.status() != WL_CONNECTED && attemptCount < 10)
	{
		delay(1000);
		Logger::log(INFO, YELLOW "." RESET_COLOR);
		attemptCount++;
	}

	if(WiFi.status() == WL_CONNECTED)
	{
		Logger::log(INFO, GREEN "Connected to WiFi" RESET_COLOR);
		Logger::log(INFO, BLUE "IP Address: " RESET_COLOR + WiFi.localIP().toString());
	}
	else
	{
		Logger::log(ERROR, RED "Failed to connect to WiFi" RESET_COLOR);
	}
}

void WiFiManager::printWiFiStatus()
{
	if(WiFi.status() == WL_CONNECTED)
	{
		Logger::log(INFO, GREEN "WiFi connected" RESET_COLOR);
		Logger::log(INFO, BLUE "IP Address: " RESET_COLOR + WiFi.localIP().toString());
	}
	else
	{
		Logger::log(ERROR, RED "WiFi not connected" RESET_COLOR);
	}
}
