// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: src/main.cpp

#include <Arduino.h>
#include <BLEManager.hpp>
#include <ChannelMap.hpp>
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
	void initializeGPIO();
	void handleButtons();

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
	initializeGPIO();

	if(isConfigured)
	{
		config.readConfig();
		setupMQTT();
		wifiManager.initialiseWiFi();
		mqttManager.connectToMQTT();
	}
	else
	{
		setupBLE();
	}
}

void MainApp::initializeGPIO()
{
	for(const auto& channel : channelMap)
	{
		pinMode(channel.second.inputPin, INPUT_PULLUP);
		pinMode(channel.second.outputPin, OUTPUT);
		digitalWrite(channel.second.outputPin, LOW);
	}
	Logger::log(INFO, GREEN "GPIO pins initialized" RESET_COLOR);
}

void MainApp::handleButtons()
{
	static unsigned long lastDebounceTime[12] = {0};
	static int lastButtonState[12] = {HIGH};
	static int buttonState[12] = {HIGH};
	static bool channelState[12] = {false};
	const unsigned long debounceDelay = 50;

	for(const auto& channel : channelMap)
	{
		int reading = digitalRead(channel.second.inputPin);

		if(reading != lastButtonState[channel.first])
		{
			lastDebounceTime[channel.first] = millis();
		}

		if((millis() - lastDebounceTime[channel.first]) > debounceDelay)
		{
			if(reading != buttonState[channel.first])
			{
				buttonState[channel.first] = reading;

				if(buttonState[channel.first] == LOW)
				{
					bool newState = !channelState[channel.first];
					channelState[channel.first] = newState;
					digitalWrite(channel.second.outputPin, newState ? HIGH : LOW);

					if(isConfigured && mqttClient.connected())
					{
						int channel_number = channel.first;
						mqttManager.publishChannelState(channel_number, newState);
					}

					Logger::log(INFO, GREEN "Button pressed: Channel " + std::to_string(channel.first) + " set to " + (newState ? "ON" : "OFF") + RESET_COLOR);
				}
			}
		}

		lastButtonState[channel.first] = reading;
	}
}

void MainApp::setupPins()
{
	pinMode(STATUS_BUZZER, OUTPUT);
	pinMode(RESET_LED, OUTPUT);
	pinMode(NVS_RESET_PIN, INPUT);
	attachInterrupt(NVS_RESET_PIN, config.resetNVS, FALLING);
	initializeGPIO();
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
	handleButtons();

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

			// It will check for incoming messages
			mqttClient.loop();
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
