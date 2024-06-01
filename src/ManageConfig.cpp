// Path: src/ManageConfig.cpp

#include "ManageConfig.h"
#include <nvs.h>
#include <nvs_flash.h>
#include <Arduino.h>
#include <iostream>
#include <sstream>

// Constructor
ManageConfig::ManageConfig()
{
    fields = {wifi_ssid, wifi_password, mqtt_server, mqtt_port, mqtt_user, mqtt_password, mqtt_topic};
}

// Getter methods
const char *ManageConfig::getWiFiSSID() const { return wifi_ssid; }
const char *ManageConfig::getWiFiPassword() const { return wifi_password; }
const char *ManageConfig::getMQTTServer() const { return mqtt_server; }
const char *ManageConfig::getMQTTPort() const { return mqtt_port; }
const char *ManageConfig::getMQTTUser() const { return mqtt_user; }
const char *ManageConfig::getMQTTPassword() const { return mqtt_password; }
const char *ManageConfig::getMQTTTopic() const { return mqtt_topic; }

// Setter methods
void ManageConfig::setWiFiSSID(const char *value) { wifi_ssid = value; }
void ManageConfig::setWiFiPassword(const char *value) { wifi_password = value; }
void ManageConfig::setMQTTServer(const char *value) { mqtt_server = value; }
void ManageConfig::setMQTTPort(const char *value) { mqtt_port = value; }
void ManageConfig::setMQTTUser(const char *value) { mqtt_user = value; }
void ManageConfig::setMQTTPassword(const char *value) { mqtt_password = value; }
void ManageConfig::setMQTTTopic(const char *value) { mqtt_topic = value; }

// This method is used to split the string received from the BLE by the delimiter (in this case, ';')
// and then make a vector of strings from the split string
std::vector<char *> ManageConfig::formatString(const char *str, char delim)
{
    std::vector<char *> tokens;
    std::stringstream ss(str);
    char *token;
    char *str_copy = strdup(str); // Make a copy of the input string
    token = strtok(str_copy, &delim);
    while (token != NULL)
    {
        tokens.push_back(token);
        token = strtok(NULL, &delim);
    }
    free(str_copy); // Free the duplicated string

    return tokens;
}

void ManageConfig::writeConfig()
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
        err = nvs_set_str(my_handle, keys[i], fields[i]);
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

void ManageConfig::readConfig()
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
            fields[i] = value;
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
