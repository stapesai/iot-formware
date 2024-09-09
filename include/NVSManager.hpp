// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file include/NVSManager.hpp
 * @brief This file contains the declaration of the NVSManager class, 
 *  which handles reading and writing configuration data to the 
 *  non-volatile storage (NVS) of the ESP32.
 */

#ifndef NVS_MANAGER_HPP
#define NVS_MANAGER_HPP

#include <Constants.hpp>
#include <array>
#include <map>
#include <nvs.h>
#include <nvs_flash.h>
#include <set>
#include <string>
#include <vector>

class NVSManager
{
private:
	nvs_handle_t nvsHandler;
	esp_err_t err;
	std::array<std::string*, 8> fields;
	std::string wifiSsid;
	std::string wifiPassword;
	std::string mqttBroker;
	std::string mqttPort;
	std::string mqttUsername;
	std::string mqttPassword;
	std::string userId;

	// Store the current state of each channel
	std::map<int, bool> channelStates;

	const char* keys[8] = {"WIFI_SSID", "WIFI_PASSWORD", "MQTT_BROKER", "MQTT_PORT", "MQTT_USERNAME", "MQTT_PASSWORD", "USER_ID"};

public:
	NVSManager();

	// Getter methods
	const std::string& getWiFiSSID() const
	{
		return wifiSsid;
	}
	const std::string& getWiFiPassword() const
	{
		return wifiPassword;
	}
	const std::string& getMQTTBroker() const
	{
		return mqttBroker;
	}
	const std::string& getMQTTPort() const
	{
		return mqttPort;
	}
	const std::string& getMQTTUsername() const
	{
		return mqttUsername;
	}
	const std::string& getMQTTPassword() const
	{
		return mqttPassword;
	}
	const std::string& getUserId() const
	{
		return userId;
	}

	// Setter methods
	void setWiFiSSID(const std::string& value)
	{
		wifiSsid = value;
	}
	void setWiFiPassword(const std::string& value)
	{
		wifiPassword = value;
	}
	void setMQTTBroker(const std::string& value)
	{
		mqttBroker = value;
	}
	void setMQTTPort(const std::string& value)
	{
		mqttPort = value;
	}
	void setMQTTUsername(const std::string& value)
	{
		mqttUsername = value;
	}
	void setMQTTPassword(const std::string& value)
	{
		mqttPassword = value;
	}
	void setUserId(const std::string& value)
	{
		userId = value;
	}

	static void resetNVS();
	bool isNVSEmpty();
	void writeConfig();
	void readConfig();
	void intialiseNVS();

	void saveChannelStates();
	void loadChannelStates();
	void updateChannelState(int channel, bool state);
	bool getChannelState(int channel) const;

	bool controlChannel(int channelNumber, bool targetState);
};

#endif // NVS_MANAGER_HPP
