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
	std::array<std::string*, 8> fields;
	std::string wifi_ssid;
	std::string wifi_password;
	std::string mqtt_broker;
	std::string mqtt_port;
	std::string mqtt_password;
	std::string user_id;
	std::string home_id;
	std::string registered_appliance_ids;

	const char* keys[8] = {"WIFI_SSID",
						   "WIFI_PASSWORD",
						   "MQTT_BROKER",
						   "MQTT_PORT",
						   "MQTT_PASSWORD",
						   "USER_ID",
						   "HOME_ID",
						   "REGISTERED_APPLIANCE_IDS"};

public:
	NVSManager();

	const std::string& getWiFiSSID() const;
	const std::string& getWiFiPassword() const;
	const std::string& getMQTTBroker() const;
	const std::string& getMQTTPort() const;
	const std::string& getMQTTPassword() const;
	const std::string& getUserId() const;
	const std::string& getHomeId() const;
	const std::string& getRegisteredApplianceIds() const;


	void setWiFiSSID(const std::string& value);
	void setWiFiPassword(const std::string& value);
	void setMQTTBroker(const std::string& value);
	void setMQTTPort(const std::string& value);
	void setMQTTPassword(const std::string& value);
	void setUserId(const std::string& value);
	void setHomeId(const std::string& value);
	void setRegisteredApplianceIds(const std::string& value);

	static void resetNVS();
	bool isNVSEmpty();
	void writeConfig();
	void readConfig();
	void intialiseNVS();
};

#endif // NVS_MANAGER_HPP
