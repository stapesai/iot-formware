// Path: src/Utils.cpp

#include <Arduino.h>
#include <Constants.hpp>
#include <Utils.hpp>
#include <nvs.h>
#include <nvs_flash.h>

std::string Utils::generateName()
{

	std::string macAddress = WiFi.macAddress().c_str();

	// Replace ':' with '_'
	std::replace(macAddress.begin(), macAddress.end(), ':', '_');

	// Create the result string
	std::string result = BLE_DEVICE_NAME + macAddress;

	return result;
}

// Format incoming input string
std::vector<std::string> Utils::formatString(const std::string& str, char delim)
{
	std::vector<std::string> tokens;
	std::stringstream ss(str);
	std::string token;
	while(std::getline(ss, token, delim))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void Utils::statusBuzzer(int times, int delayTime)
{
	for(int i = 0; i < times; i++)
	{
		digitalWrite(STATUS_BUZZER, HIGH);
		delay(delayTime);
		digitalWrite(STATUS_BUZZER, LOW);
		delay(delayTime);
	}
}

void Utils::printConfig(const NVSManager& config)
{
	Logger::log(DEBUG, GREEN "SSID: " + config.getWiFiSSID() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "Password: " + config.getWiFiPassword() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "MQTT Server: " + config.getMQTTBroker() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "MQTT Port: " + config.getMQTTPort() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "MQTT Password: " + config.getMQTTPassword() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "MQTT Topic: " + config.getUserId() + RESET_COLOR);
}

void Utils::restartDevice(unsigned long delayTime)
{
	Logger::log(INFO, YELLOW "Restarting device...");
	delay(delayTime);
	esp_restart();
}