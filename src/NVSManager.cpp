// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: src/NVSManager.cpp

#include <Arduino.h>
#include <ChannelMap.hpp>
#include <Constants.hpp>
#include <Logger.hpp>
#include <NVSManager.hpp>
#include <Utils.hpp>

// Constructor
NVSManager::NVSManager()
{
	fields = {&wifi_ssid, &wifi_password, &mqtt_broker, &mqtt_port, &mqtt_username, &mqtt_password, &user_id};
	channelStates.clear();
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

void NVSManager::saveChannelStates()
{
	if(nvs_open("storage", NVS_READWRITE, &nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle for channel states" RESET_COLOR);
		return;
	}

	for(const auto& channel : channelMap)
	{
		std::string key = "CH_" + std::to_string(channel.first);
		bool state = channelStates[channel.first];
		if(nvs_set_u8(nvs_handler, key.c_str(), state ? 1 : 0) != ESP_OK)
		{
			Logger::log(ERROR, RED "Error writing channel state to NVS" RESET_COLOR);
		}
	}

	if(nvs_commit(nvs_handler) == ESP_OK)
	{
		Logger::log(INFO, GREEN "Channel states committed to NVS successfully" RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Error committing channel states to NVS" RESET_COLOR);
	}
	nvs_close(nvs_handler);
}

void NVSManager::loadChannelStates()
{
	if(nvs_open("storage", NVS_READONLY, &nvs_handler) != ESP_OK)
	{
		Logger::log(ERROR, RED "Error opening NVS handle for channel states" RESET_COLOR);
		return;
	}

	for(const auto& channel : channelMap)
	{
		std::string key = "CH_" + std::to_string(channel.first);
		uint8_t state;
		if(nvs_get_u8(nvs_handler, key.c_str(), &state) == ESP_OK)
		{
			channelStates[channel.first] = (state == 1);
			digitalWrite(channel.second.outputPin, channelStates[channel.first] ? HIGH : LOW);
			Logger::log(INFO, GREEN "Loaded channel " + std::to_string(channel.first) + " state: " + std::to_string(state) + RESET_COLOR);
		}
		else
		{
			channelStates[channel.first] = false;
			Logger::log(WARN, YELLOW "Channel " + std::to_string(channel.first) + " state not found in NVS, defaulting to OFF" RESET_COLOR);
		}
	}

	nvs_close(nvs_handler);
}

void NVSManager::updateChannelState(int channel, bool state)
{
	channelStates[channel] = state;
	saveChannelStates();
}

bool NVSManager::getChannelState(int channel) const
{
	auto it = channelStates.find(channel);
	if(it != channelStates.end())
	{
		return it->second;
	}
	return false; // Default to OFF if channel is not found
}

bool NVSManager::controlChannel(int channel_number, bool target_state)
{
	auto it = channelMap.find(channel_number);
	if(it == channelMap.end())
	{
		Logger::log(ERROR, RED "Invalid channel number" RESET_COLOR);
		return false;
	}

	digitalWrite(it->second.outputPin, target_state ? HIGH : LOW);
	updateChannelState(channel_number, target_state);
	Logger::log(INFO,
				GREEN "Channel " + std::to_string(channel_number) + " (pin " + std::to_string(it->second.outputPin) + ") set to " + (target_state ? "ON" : "OFF") +
					RESET_COLOR);
	return true;
}
