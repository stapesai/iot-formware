// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: include/main.hpp

#ifndef MAIN_HPP
#define MAIN_HPP

#include <Arduino.h>
#include <BLEManager.hpp>
#include <ChannelMap.hpp>
#include <Constants.hpp>
#include <MQTTManager.hpp>
#include <WiFiManager.hpp>
#include <map>
#include <mutex>

class MainApp
{
public:
	MainApp();
	void setup();
	void loop();

private:
	void setupPins();
	void setupNVS();
	void setupBLE();
	void setupMQTT();
	void checkWiFiAndMQTT(unsigned long);
	void initializeGPIO();
	void handleButtons();
	void loadChannelStates();
	void publishInitialStates();

	bool isConfigured;
	unsigned long previousMillisWiFi;
	unsigned long previousMillisMQTT;
	unsigned long keepAliveInterval;
	unsigned long requiredInterval;
	unsigned long keep_alive_counter;

	Utils utils;
	NVSManager config;
	WiFiClient wifiClient;
	PubSubClient mqttClient;
	MQTTManager mqttManager;
	BLEManager bleManager;
	WiFiManager wifiManager;
};

#endif