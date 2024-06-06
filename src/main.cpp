// Path: src/main.cpp

#include <Arduino.h>
#include <BLEManager.hpp>
#include <Constants.hpp>
#include <Logger.hpp>
#include <MQTTManager.hpp>
#include <PubSubClient.h>
#include <WiFiManager.hpp>

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

	bool isConfigured;
	unsigned long previousMillisWiFi;
	unsigned long previousMillisMQTT;
	unsigned long previousMillisKeepAlive;
	unsigned long keepAliveInterval;
	unsigned long requiredInterval;
	unsigned long keep_alive_counter;

	Utils utils; // Utility class to handle common functions
	NVSManager config; // NVS manager class
	WiFiClient wifiClient; // WiFi client for MQTT
	PubSubClient mqttClient; // MQTT client
	MQTTManager mqttManager; // MQTT manager
	BLEManager bleManager; // BLE Config Callbacks
	WiFiManager wifiManager; // WiFi Manager
};

MainApp::MainApp()
	: previousMillisWiFi(0)
	, previousMillisMQTT(0)
	, keepAliveInterval(50000)
	, requiredInterval(5000)
	, keep_alive_counter(1)
	, isConfigured(false)
	, mqttClient(wifiClient)
	, mqttManager(&mqttClient, &config, &utils)
	, bleManager(config, utils)
	, wifiManager(&config)
{ }

void MainApp::setup()
{
	Serial.begin(115200);
	setupPins();
	setupNVS();

	if(isConfigured)
	{
		config.readConfig();
		setupMQTT();
	}
	else
	{
		setupBLE();
	}
}

void MainApp::setupPins()
{
	pinMode(STATUS_BUZZER, OUTPUT);
	pinMode(RESET_LED, OUTPUT);
	pinMode(NVS_RESET_PIN, INPUT);
	attachInterrupt(NVS_RESET_PIN, config.resetNVS, FALLING);
}

void MainApp::setupNVS()
{
	config.intialiseNVS();
	isConfigured = !config.isNVSEmpty();
}

void MainApp::setupBLE()
{
	bleManager.initializeBLE();
	Logger::log(INFO, BLUE "Data is not present in NVS" RESET_COLOR);
	Logger::log(INFO, BLUE "Waiting for a client connection to notify..." RESET_COLOR);
	Logger::log(INFO, BLUE "The device started, now you can pair it with BLE Scanner!" RESET_COLOR);
}

void MainApp::setupMQTT()
{
	mqttManager.initializeMQTT();
}

void MainApp::loop()
{
	unsigned long currentMillis = millis();
	if(isConfigured)
	{
		checkWiFiAndMQTT(currentMillis);
	}
	else
	{
		if(currentMillis - previousMillisWiFi >= requiredInterval)
		{
			previousMillisWiFi = currentMillis;
			Logger::log(INFO, YELLOW "Waiting for a client connection to notify..." RESET_COLOR);
		}
	}
}

void MainApp::checkWiFiAndMQTT(unsigned long currentMillis)
{
	if(WiFi.status() == WL_CONNECTED)
	{
		if(mqttClient.connected())
		{
			if(currentMillis - previousMillisMQTT >= keepAliveInterval)
			{
				previousMillisMQTT = currentMillis;
				Logger::log(INFO, MAGENTA "Sending keep alive message" RESET_COLOR);
				mqttManager.publish_keep_alive_message(keep_alive_counter);
				keep_alive_counter++;
			}
		}
		else
		{
			if(currentMillis - previousMillisMQTT >= requiredInterval)
			{
				previousMillisMQTT = currentMillis;
				keep_alive_counter = 0;
				Logger::log(WARN, ORANGE "Reconnecting to MQTT..." RESET_COLOR);
				mqttManager.connectToMQTT();
			}
		}
	}
	else
	{
		if(currentMillis - previousMillisWiFi >= requiredInterval)
		{
			previousMillisWiFi = currentMillis;
			keep_alive_counter = 0;
			Logger::log(WARN, ORANGE "Reconnecting to WiFi..." RESET_COLOR);
			wifiManager.initialiseWiFi();
		}
	}
}

MainApp app;

void setup()
{
	app.setup();
	
}

void loop()
{
	app.loop();
}
