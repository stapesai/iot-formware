// Path: src/NVSManager.cpp

#include <Arduino.h>
#include <Constants.hpp>
#include <Logger.hpp>
#include <NVSManager.hpp>
#include <Utils.hpp>

// Constructor
NVSManager::NVSManager()
{
	fields = {
		&wifi_ssid, &wifi_password, &mqtt_broker, &mqtt_port, &mqtt_password, &user_id, &home_id};
}

// Getter methods
const std::string& NVSManager::getWiFiSSID() const
{
	return wifi_ssid;
}
const std::string& NVSManager::getWiFiPassword() const
{
	return wifi_password;
}
const std::string& NVSManager::getMQTTBroker() const
{
	return mqtt_broker;
}
const std::string& NVSManager::getMQTTPort() const
{
	return mqtt_port;
}

const std::string& NVSManager::getMQTTPassword() const
{
	return mqtt_password;
}
const std::string& NVSManager::getUserId() const
{
	return user_id;
}

const std::string& NVSManager::getHomeId() const
{
	return home_id;
}

// Setter methods
void NVSManager::setWiFiSSID(const std::string& value)
{
	wifi_ssid = value;
}
void NVSManager::setWiFiPassword(const std::string& value)
{
	wifi_password = value;
}
void NVSManager::setMQTTBroker(const std::string& value)
{
	mqtt_broker = value;
}
void NVSManager::setMQTTPort(const std::string& value)
{
	mqtt_port = value;
}

void NVSManager::setMQTTPassword(const std::string& value)
{
	mqtt_password = value;
}
void NVSManager::setUserId(const std::string& value)
{
	user_id = value;
}
void NVSManager::setHomeId(const std::string& value)
{
	home_id = value;
}
// Interrupt handler to reset the module
void IRAM_ATTR NVSManager::resetNVS()
{
	if(digitalRead(NVS_RESET_PIN) == LOW)
	{
		if(nvs_flash_erase() == ESP_OK)
		{
			Logger::log(INFO, CYAN "NVS erased" RESET_COLOR);
		}
		else
		{
			Logger::log(ERROR, RED "Error erasing NVS" RESET_COLOR);
		}
		Logger::log(INFO, YELLOW "Restarting ESP32" RESET_COLOR);
		delay(1000);
		esp_restart();
	}
}

bool NVSManager::isNVSEmpty()
{
	err = nvs_open("storage", NVS_READWRITE, &nvs_handler);
	for(int i = 0; i < 6; i++)
	{
		// Check NVS stats
		size_t required_size;
		err = nvs_get_str(nvs_handler, keys[i], NULL, &required_size);
		nvs_close(nvs_handler);

		if(err == ESP_OK)
		{
			Logger::log(DEBUG, GREEN "Key: " + String(keys[i]) + " exists" RESET_COLOR);
			return false;
		}
		else if(err == ESP_ERR_NVS_NOT_FOUND)
		{
			return true;
		}
		else
		{
			Logger::log(ERROR,
						RED "Error getting key '" + String(keys[i]) +
							"': " + String(esp_err_to_name(err)) + RESET_COLOR);
			return true;
		}
	}
	return true;
}

void NVSManager::intialiseNVS()
{
	err = nvs_flash_init();

	// Check if initialization encountered specific errors
	if(err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		// NVS partition was truncated and needs to be erased
		ESP_ERROR_CHECK(nvs_flash_erase());
		// Re-initialize NVS after erasing the partition
		err = nvs_flash_init();
	}

	ESP_ERROR_CHECK(err);
}

void NVSManager::writeConfig()
{
	if(nvs_open("storage", NVS_READWRITE, &nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle" RESET_COLOR);
		return;
	}

	for(int i = 0; i < 6; ++i)
	{
		Logger::log(INFO,
					YELLOW "Writing to NVS: " + String(keys[i]) + " = " +
						String(fields[i]->c_str()) + RESET_COLOR);
		if(nvs_set_str(nvs_handler, keys[i], fields[i]->c_str()) != ESP_OK)
		{
			Logger::log(ERROR, RED "Error writing to NVS" RESET_COLOR);
		}
	}

	if(nvs_commit(nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error committing NVS changes" RESET_COLOR);
	}
	else
	{
		Logger::log(INFO, GREEN "NVS committed successfully" RESET_COLOR);
	}
	nvs_close(nvs_handler);
}

void NVSManager::readConfig()
{

	err = nvs_open("storage", NVS_READWRITE, &nvs_handler);

	if(err != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle" RESET_COLOR);
		return;
	}
	else
	{
		Logger::log(INFO, GREEN "NVS opened	successfully" RESET_COLOR);
	}

	for(int i = 0; i < 6; ++i)
	{
		size_t required_size;
		err = nvs_get_str(nvs_handler, keys[i], NULL, &required_size);
		if(err != ESP_OK)
		{
			Logger::log(ERROR,
						RED "Error reading size for key '%s'\n" + String(keys[i]) + RESET_COLOR);
			continue;
		}

		char* value = (char*)malloc(required_size);
		if(value == nullptr)
		{
			Logger::log(ERROR,
						RED "Error allocating memory for key '%s'\n" + String(keys[i]) +
							RESET_COLOR);
			continue;
		}

		err = nvs_get_str(nvs_handler, keys[i], value, &required_size);
		if(err == ESP_OK)
		{
			*fields[i] = std::string(value);
			Logger::log(DEBUG,
						GREEN "Read from NVS: " + String(keys[i]) + " = " + String(value) +
							RESET_COLOR);
		}
		else
		{
			Logger::log(ERROR,
						RED "Error reading value for key '%s'\n" + String(keys[i]) + RESET_COLOR);
		}
		free(value);
	}
	nvs_close(nvs_handler);
}
