// Path: src/Utils.cpp

#include <Arduino.h>
#include <Constants.hpp>
#include <Utils.hpp>
#include <nvs.h>
#include <nvs_flash.h>

std::string Utils::getMacAddr()
{

	std::string macAddress = WiFi.macAddress().c_str();

	// Replace ':' with '_'
	std::replace(macAddress.begin(), macAddress.end(), ':', '_');

	// Create the result string
	std::string result = macAddress;
	// std::string result = BLE_DEVICE_NAME + macAddress;

	return result;
}

// Format incoming input string as key-value pairs
std::map<std::string, std::string> Utils::convertToKVMap(const std::string& str)
{
	char delim = ';';
	char kv_delim = '=';

	std::map<std::string, std::string> kv_map;
	std::stringstream ss(str);
	std::string token;
	while(std::getline(ss, token, delim))
	{
		size_t pos = token.find(kv_delim);
		if(pos != std::string::npos)
		{
			std::string key = token.substr(0, pos);
			std::string value = token.substr(pos + 1);
			kv_map[key] = value;
		}
	}
	return kv_map;
}

// Function to convert key-value pairs to a string
std::string Utils::convertToKVString(const std::map<std::string, std::string>& kv_map)
{
	std::stringstream ss;
	for(const auto& pair : kv_map)
	{
		ss << pair.first << "=" << pair.second << ";";
	}
	// Remove the trailing semicolon if there is one
	std::string kv_string = ss.str();
	if(!kv_string.empty() && kv_string.back() == ';')
	{
		kv_string.pop_back();
	}
	return kv_string;
}

// It is used to split a string into a vector of sub-strings based on a delimiter
// (e.g., splitString("hello,world", ',') returns ["hello", "world"]
std::vector<std::string> Utils::splitString(std::string string, char delim)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(string);
	while(std::getline(tokenStream, token, delim))
	{
		tokens.push_back(token);
	}
	return tokens;
}

// Function to generate a buzzer sound
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

// function generate the mqtt topic
// std::string Utils::generateTopic(std::string& user_id, std::string& home_id, std::string& client_id)
// {
// 	std::ostringstream base_mqtt_topic;
// 	base_mqtt_topic << "user/" << user_id << "/home/" << home_id << "/node/" << client_id << "/";
// 	std::string keep_alive_request = base_mqtt_topic.str() + "/keepalive/request";
// 	std::string device_registration_request = base_mqtt_topic.str() + "/device/register/request";
// 	std::string device_unregistration_request =
// 		base_mqtt_topic.str() + "/device/unregister/request";
// 	std::string device_control_request = base_mqtt_topic.str() + "/device/control/request";
// 	std::cout << "Keep alive request topic: " << keep_alive_request << std::endl;
// }

void Utils::printConfig(const NVSManager& config)
{
	Logger::log(DEBUG, GREEN "SSID: " + config.getWiFiSSID() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "Password: " + config.getWiFiPassword() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "MQTT Server: " + config.getMQTTBroker() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "MQTT Port: " + config.getMQTTPort() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "MQTT Password: " + config.getMQTTPassword() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "User ID: " + config.getUserId() + RESET_COLOR);
	Logger::log(DEBUG, GREEN "Home ID: " + config.getHomeId() + RESET_COLOR);
}

void Utils::restartDevice(unsigned long delayTime)
{
	Logger::log(INFO, YELLOW "Restarting device...");
	delay(delayTime);
	esp_restart();
}