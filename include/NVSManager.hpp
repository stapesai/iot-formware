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
	std::string reg_device_ids;

	const char* keys[8] = {"WIFI_SSID",
						   "WIFI_PASSWORD",
						   "MQTT_BROKER",
						   "MQTT_PORT",
						   "MQTT_PASSWORD",
						   "USER_ID",
						   "HOME_ID",
						   "REG_DEVICE_IDS"};

public:
	NVSManager();

	// Getter methods
    const std::string& getWiFiSSID() const { return wifi_ssid; }
    const std::string& getWiFiPassword() const { return wifi_password; }
    const std::string& getMQTTBroker() const { return mqtt_broker; }
    const std::string& getMQTTPort() const { return mqtt_port; }
    const std::string& getMQTTPassword() const { return mqtt_password; }
    const std::string& getUserId() const { return user_id; }
    const std::string& getHomeId() const { return home_id; }
    const std::string& getRegisteredDevicesIds() const { return reg_device_ids; }

    // Setter methods
    void setWiFiSSID(const std::string& value) { wifi_ssid = value; }
    void setWiFiPassword(const std::string& value) { wifi_password = value; }
    void setMQTTBroker(const std::string& value) { mqtt_broker = value; }
    void setMQTTPort(const std::string& value) { mqtt_port = value; }
    void setMQTTPassword(const std::string& value) { mqtt_password = value; }
    void setUserId(const std::string& value) { user_id = value; }
    void setHomeId(const std::string& value) { home_id = value; }
    void setRegisteredDeviceIds(const std::string& value) { reg_device_ids = value; }

	bool registerNewDevice(const std::string& device_id, const std::string& channel_number);
	bool unregisterDevice(const std::string& device_id);

	static void resetNVS();
	bool isNVSEmpty();
	void writeConfig();
	void readConfig();
	void intialiseNVS();
};

#endif // NVS_MANAGER_HPP
