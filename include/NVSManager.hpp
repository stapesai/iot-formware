// Path: include/NVSManager.hpp

#ifndef NVS_MANAGER_HPP
#define NVS_MANAGER_HPP

#include <Constants.hpp>
#include <array>
#include <nvs.h>
#include <nvs_flash.h>
#include <string>
#include <vector>

class NVSManager
{
private:
	nvs_handle_t nvs_handler;
	esp_err_t err;
	std::string wifi_ssid;
	std::string wifi_password;
	std::string mqtt_broker;
	std::string mqtt_port;
	std::string mqtt_password;
	std::string user_id;
	std::array<std::string*, 6> fields;
	const char* keys[7] = {"wifi_ssid",
						   "wifi_password",
						   "mqtt_broker",
						   "mqtt_port",
						   "mqtt_password",
						   "user_id"};

public:
	NVSManager();

	const std::string& getWiFiSSID() const;
	const std::string& getWiFiPassword() const;
	const std::string& getMQTTBroker() const;
	const std::string& getMQTTPort() const;
	const std::string& getMQTTPassword() const;
	const std::string& getUserId() const;

	void setWiFiSSID(const std::string& value);
	void setWiFiPassword(const std::string& value);
	void setMQTTBroker(const std::string& value);
	void setMQTTPort(const std::string& value);
	void setMQTTPassword(const std::string& value);
	void setUserId(const std::string& value);

	static void resetNVS();
	bool isNVSEmpty();
	void writeConfig();
	void readConfig();
	void intialiseNVS();
};

#endif // NVS_MANAGER_HPP
