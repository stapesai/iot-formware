// Path: src/NVSManager.cpp

#include <NVSManager.h>
#include <nvs.h>
#include <nvs_flash.h>
#include <Arduino.h>

#include <utils.h>

// Constructor
NVSManager::NVSManager()
{
    fields = {&wifi_ssid, &wifi_password, &mqtt_broker, &mqtt_port, &mqtt_user, &mqtt_password, &mqtt_topic};
}

// Getter methods
const std::string &NVSManager::getWiFiSSID() const { return wifi_ssid; }
const std::string &NVSManager::getWiFiPassword() const { return wifi_password; }
const std::string &NVSManager::getMQTTBroker() const { return mqtt_broker; }
const std::string &NVSManager::getMQTTPort() const { return mqtt_port; }
const std::string &NVSManager::getMQTTUser() const { return mqtt_user; }
const std::string &NVSManager::getMQTTPassword() const { return mqtt_password; }
const std::string &NVSManager::getMQTTTopic() const { return mqtt_topic; }

// Setter methods
void NVSManager::setWiFiSSID(const std::string &value) { wifi_ssid = value.c_str(); }
void NVSManager::setWiFiPassword(const std::string &value) { wifi_password = value; }
void NVSManager::setMQTTBroker(const std::string &value) { mqtt_broker = value; }
void NVSManager::setMQTTPort(const std::string &value) { mqtt_port = value; }
void NVSManager::setMQTTUser(const std::string &value) { mqtt_user = value; }
void NVSManager::setMQTTPassword(const std::string &value) { mqtt_password = value; }
void NVSManager::setMQTTTopic(const std::string &value) { mqtt_topic = value; }

// Interrupt handler to reset the module
void IRAM_ATTR NVSManager::resetNVS()
{
    if (digitalRead(NVS_RESET_PIN) == LOW)
    {
        if (nvs_flash_erase() == ESP_OK)
        {
            Serial.println("NVS erased");
        }
        else
        {
            Serial.println("Error erasing NVS");
        }

        Serial.println("Restarting ESP32...");
        delay(1000);
        esp_restart();
    }
}

bool NVSManager::isNVSEmpty()
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open_from_partition("storage", "nvs", NVS_READONLY, &my_handle);

    // Check NVS stats
    nvs_stats_t nvs_stats;
    err = nvs_get_stats(NULL, &nvs_stats);
    if (err != ESP_OK)
    {
        Serial.println("Error getting NVS stats");
        return false;
    }

    // Check if used entries are zero
    return nvs_stats.used_entries == 0;
}

void NVSManager::intialiseNVS()
{
    esp_err_t err = nvs_flash_init();

    // Check if initialization encountered specific errors
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // If NVS initialization encountered errors related to free pages or new version found,
        // erase the entire NVS partition to resolve these issues
        ESP_ERROR_CHECK(nvs_flash_erase());
        // Re-initialize NVS after erasing the partition
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
}

void NVSManager::writeConfig()
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        Serial.println("Error opening NVS handle");
        return;
    }

    for (int i = 0; i < 7; ++i)
    {
        err = nvs_set_str(my_handle, keys[i], fields[i]->c_str());
        if (err != ESP_OK)
        {
            Serial.printf("Error writing to NVS for key '%s'\n", keys[i]);
        }
    }

    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        Serial.println("Error committing NVS changes");
    }
    nvs_close(my_handle);
}

void NVSManager::readConfig()
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        Serial.println("Error opening NVS handle");
        return;
    }

    for (int i = 0; i < 7; ++i)
    {
        size_t required_size;
        err = nvs_get_str(my_handle, keys[i], NULL, &required_size);
        if (err != ESP_OK)
        {
            // Serial.printf("Error reading size for key '%s'\n", keys[i]);
            continue;
        }

        char *value = (char *)malloc(required_size);
        if (value == nullptr)
        {
            Serial.println("Failed to allocate memory for NVS read");
            continue;
        }

        err = nvs_get_str(my_handle, keys[i], value, &required_size);
        if (err == ESP_OK)
        {
            *fields[i] = std::string(value);
            Serial.printf("Read from NVS: %s = %s\n", keys[i], value);
        }
        else
        {
            Serial.printf("Error reading value for key '%s'\n", keys[i]);
        }
        free(value);
    }
    nvs_close(my_handle);
}
