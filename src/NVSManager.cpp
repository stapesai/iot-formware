// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: src/NVSManager.cpp

#include <Arduino.h>
#include <Constants.hpp>
#include <Logger.hpp>
#include <NVSManager.hpp>
#include <Utils.hpp>

// Constructor
NVSManager::NVSManager()
{
	fields = {&wifi_ssid, &wifi_password, &mqtt_broker, &mqtt_port, &mqtt_username, &mqtt_password, &user_id};
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
	if(nvs_open("storage", NVS_READONLY, &nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle" RESET_COLOR);
		return true;
	}
	Logger::log(INFO, GREEN "Checking if NVS is empty" RESET_COLOR);
	bool allKeysMissing = true;

	for(int i = 0; i < 7; i++)
	{
		// Check NVS stats
		size_t required_size;
		err = nvs_get_str(nvs_handler, keys[i], NULL, &required_size);

		if(err == ESP_OK)
		{
			Logger::log(DEBUG, GREEN "Key: " + std::string(keys[i]) + " exists" RESET_COLOR);
			allKeysMissing = false; // At least one key exists
		}
		else if(err == ESP_ERR_NVS_NOT_FOUND)
		{
			Logger::log(WARN, ORANGE "Key: " + std::string(keys[i]) + " does not exist" RESET_COLOR);
		}
		else
		{
			Logger::log(ERROR, RED "Error getting key '" + std::string(keys[i]) + "': " + std::string(esp_err_to_name(err)) + RESET_COLOR);
			allKeysMissing = true;
			break; // Exit loop on other errors
		}
	}

	nvs_close(nvs_handler);

	return allKeysMissing;
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

	for(int i = 0; i < 7; i++)
	{
		Logger::log(INFO, YELLOW "Writing to NVS: " + String(keys[i]) + " = " + String(fields[i]->c_str()) + RESET_COLOR);
		if(nvs_set_str(nvs_handler, keys[i], fields[i]->c_str()) != ESP_OK)
		{
			Logger::log(ERROR, RED "Error writing to NVS" RESET_COLOR);
		}
	}

	if(nvs_commit(nvs_handler) == ESP_OK)
	{
		Logger::log(INFO, GREEN "NVS committed successfully" RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Error committing NVS changes" RESET_COLOR);
	}
	nvs_close(nvs_handler);
}

void NVSManager::readConfig()
{
	if(nvs_open("storage", NVS_READWRITE, &nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle" RESET_COLOR);
		return;
	}
	Logger::log(INFO, GREEN "NVS opened	successfully" RESET_COLOR);

	for(int i = 0; i < 7; i++)
	{
		size_t required_size;
		err = nvs_get_str(nvs_handler, keys[i], NULL, &required_size);
		if(err != ESP_OK)
		{
			Logger::log(ERROR, RED "Error reading size for key '%s'\n" + String(keys[i]) + RESET_COLOR);
			continue;
		}

		char* value = (char*)malloc(required_size);
		if(value == nullptr)
		{
			Logger::log(ERROR, RED "Error allocating memory for key '%s'\n" + String(keys[i]) + RESET_COLOR);
			continue;
		}

		err = nvs_get_str(nvs_handler, keys[i], value, &required_size);
		if(err == ESP_OK)
		{
			*fields[i] = std::string(value);
			Logger::log(DEBUG, GREEN "Read from NVS: " + String(keys[i]) + " = " + String(value) + RESET_COLOR);
		}
		else
		{
			Logger::log(ERROR, RED "Error reading value for key '%s'\n" + String(keys[i]) + RESET_COLOR);
		}
		free(value);
	}
	nvs_close(nvs_handler);
}

bool NVSManager::registerNewDevice(const std::string& device_id, const std::string& channel_number)
{
	// Check both device_id and channel_number are not empty
	if(device_id.empty() || channel_number.empty())
	{
		Logger::log(ERROR, RED "Device ID or channel number is empty" RESET_COLOR);
		return false;
	}

	// Make a new key-value pair in NVS with KEY=device_id and VALUE=channel_number
	nvs_handle_t nvs_handler;

	if(nvs_open("storage", NVS_READWRITE, &nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle" RESET_COLOR);
		return false;
	}

	// If the key already exists, return false
	size_t required_size;
	if(nvs_get_str(nvs_handler, device_id.c_str(), NULL, &required_size) == ESP_OK)
	{
		Logger::log(ERROR, RED "Device ID already exists in NVS: " + String(device_id.c_str()) + RESET_COLOR);
		return false;
	}

	// If the key does not exist, write the key-value pair
	if(nvs_set_str(nvs_handler, device_id.c_str(), channel_number.c_str()) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error writing to NVS" RESET_COLOR);
		return false;
	}

	// Commit the changes
	if(nvs_commit(nvs_handler) == ESP_OK)
	{
		Logger::log(INFO, GREEN "NVS committed successfully" RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Error committing NVS changes" RESET_COLOR);
		return false;
	}

	// Close the NVS handle and return true
	nvs_close(nvs_handler);
	return true;
}

bool NVSManager::unregisterDevice(const std::string& device_id)
{
	// Check both device_id is empty
	if(device_id.empty())
	{
		Logger::log(ERROR, RED "Device ID or channel number is empty" RESET_COLOR);
		return false;
	}
	// Remove the key-value pair from NVS with KEY=device_id
	nvs_handle_t nvs_handler;
	if(nvs_open("storage", NVS_READWRITE, &nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle" RESET_COLOR);
		return false;
	}

	// If the key does not exist, return false
	size_t required_size;
	if(nvs_get_str(nvs_handler, device_id.c_str(), NULL, &required_size) != ESP_OK)
	{
		Logger::log(ERROR, RED "Device ID does not exist in NVS: " + String(device_id.c_str()) + RESET_COLOR);
		return false;
	}

	// If the key exists, erase the key
	if(nvs_erase_key(nvs_handler, device_id.c_str()) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error erasing key from NVS" RESET_COLOR);
		return false;
	}

	// Commit the changes
	if(nvs_commit(nvs_handler) == ESP_OK)
	{
		Logger::log(INFO, GREEN "NVS committed successfully" RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Error committing NVS changes" RESET_COLOR);
		return false;
	}

	// Close the NVS handle and return true
	nvs_close(nvs_handler);
	return true;
}

std::string NVSManager::getChannelNumber(const std::string& device_id)
{
	nvs_handle_t nvs_handler;
	if(nvs_open("storage", NVS_READONLY, &nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle" RESET_COLOR);
		return ""; // Return empty string if NVS open fails
	}

	size_t required_size;
	esp_err_t err = nvs_get_str(nvs_handler, device_id.c_str(), nullptr, &required_size);
	if(err != ESP_OK)
	{
		Logger::log(ERROR, RED "Error reading size for key '%s'\n" + device_id + RESET_COLOR + esp_err_to_name(err));
		nvs_close(nvs_handler);
		return ""; // Return empty string if key size read fails
	}

	// Allocate memory for value
	char* value = (char*)malloc(required_size);
	if(value == nullptr)
	{
		Logger::log(ERROR, RED "Error allocating memory for key '%s'\n" + device_id + RESET_COLOR);
		nvs_close(nvs_handler);
		return ""; // Return empty string if memory allocation fails
	}

	// Read the value
	err = nvs_get_str(nvs_handler, device_id.c_str(), value, &required_size);
	if(err == ESP_OK)
	{
		std::string result(value);
		Logger::log(DEBUG, GREEN "Read from NVS: " + device_id + " = " + result + RESET_COLOR);
		free(value); // Free allocated memory
		nvs_close(nvs_handler);
		return result; // Return the value as a string
	}
	else
	{
		Logger::log(ERROR, RED "Error reading value for key '%s': %s\n" + device_id + RESET_COLOR + esp_err_to_name(err));
		free(value); // Free allocated memory
		nvs_close(nvs_handler);
		return ""; // Return empty string if value read fails
	}
}

bool NVSManager::controlDevice(const std::string& device_id, const std::string& target_state)
{
	// Check both device_id and target_state are not empty
	if(device_id.empty() || target_state.empty())
	{
		Logger::log(ERROR, RED "Device ID or target state is empty" RESET_COLOR);
		return false;
	}

	// Control the device
	// For now, just print the device_id and target_state
	// TODO: Implement actual device control
	Logger::log(INFO, GREEN "Device ID: " + device_id + RESET_COLOR);
	Logger::log(INFO, GREEN "Target State: " + target_state + RESET_COLOR);

	// Return true if control is successful
	return true;
}
