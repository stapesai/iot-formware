// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: include/NVSManager.hpp

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
	nvs_handle_t nvs_handler;
	esp_err_t err;
	// std::set<std::string> registeredDeviceIds;
	// std::vector<std::pair<std::string, std::string>> registeredDevices;
	std::array<std::string*, 8> fields;
	std::string wifi_ssid;
	std::string wifi_password;
	std::string mqtt_broker;
	std::string mqtt_port;
	std::string mqtt_username;
	std::string mqtt_password;
	std::string user_id;

	const char* keys[8] = {"WIFI_SSID", "WIFI_PASSWORD", "MQTT_BROKER", "MQTT_PORT", "MQTT_USERNAME", "MQTT_PASSWORD", "USER_ID"};

public:
	NVSManager();

	// Getter methods
	const std::string& getWiFiSSID() const
	{
		return wifi_ssid;
	}
	const std::string& getWiFiPassword() const
	{
		return wifi_password;
	}
	const std::string& getMQTTBroker() const
	{
		return mqtt_broker;
	}
	const std::string& getMQTTPort() const
	{
		return mqtt_port;
	}
	const std::string& getMQTTUsername() const
	{
		return mqtt_username;
	}
	const std::string& getMQTTPassword() const
	{
		return mqtt_password;
	}
	const std::string& getUserId() const
	{
		return user_id;
	}

	// Setter methods
	void setWiFiSSID(const std::string& value)
	{
		wifi_ssid = value;
	}
	void setWiFiPassword(const std::string& value)
	{
		wifi_password = value;
	}
	void setMQTTBroker(const std::string& value)
	{
		mqtt_broker = value;
	}
	void setMQTTPort(const std::string& value)
	{
		mqtt_port = value;
	}
	void setMQTTUsername(const std::string& value)
	{
		mqtt_username = value;
	}
	void setMQTTPassword(const std::string& value)
	{
		mqtt_password = value;
	}
	void setUserId(const std::string& value)
	{
		user_id = value;
	}

	bool registerNewDevice(const std::string& device_id, uint8_t channel_number);
	bool unregisterDevice(const std::string& device_id);
	std::string getChannelNumber(const std::string& device_id);
	bool controlDevice(const std::string& device_id, bool target_state);

	static void resetNVS();
	bool isNVSEmpty();
	void writeConfig();
	void readConfig();
	void intialiseNVS();
};

#endif // NVS_MANAGER_HPP
