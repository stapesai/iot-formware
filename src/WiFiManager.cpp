// Path: src/WiFiManager.cpp

#include "WiFiManager.hpp"

WiFiManager::WiFiManager(NVSManager* config)
	: config(config)
{ }

void WiFiManager::initialiseWiFi()
{
	Serial.println("Initializing WiFi...");
	setupWiFi();
	connectWiFi();
	printWiFiStatus();
}

void WiFiManager::setupWiFi()
{
	WiFi.mode(WIFI_STA);
	Serial.print("Setting up WiFi with SSID: ");
	Serial.println(config->getWiFiSSID().c_str());
}

void WiFiManager::connectWiFi()
{
	Serial.print("Connecting to WiFi SSID: ");
	Serial.println(config->getWiFiSSID().c_str());
	Serial.print("With password: ");
	Serial.println(config->getWiFiPassword().c_str());

	WiFi.begin(config->getWiFiSSID().c_str(), config->getWiFiPassword().c_str());

	uint8_t attemptCount = 0;
	while(WiFi.status() != WL_CONNECTED && attemptCount < 10)
	{
		delay(1000);
		Serial.print(".");
		attemptCount++;
	}

	Serial.println();
	if(WiFi.status() == WL_CONNECTED)
	{
		Serial.println("Connected to the WiFi network");
		Serial.print("IP Address: ");
		Serial.println(WiFi.localIP());
	}
	else
	{
		Serial.println("Failed to connect to WiFi");
	}
}

void WiFiManager::printWiFiStatus()
{
	if(WiFi.status() == WL_CONNECTED)
	{
		Serial.println("WiFi connected");
		Serial.print("IP Address: ");
		Serial.println(WiFi.localIP());
	}
	else
	{
		Serial.println("WiFi not connected");
	}
}
