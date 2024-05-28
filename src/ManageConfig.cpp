#include "ManageConfig.h"
#include <nvs.h>
#include <nvs_flash.h>
#include <Arduino.h>
#include <iostream>
#include <sstream>

// Constructor
ManageConfig::ManageConfig()
{
    fields = {&ssid, &password, &mqtt_server, &mqtt_port, &mqtt_user, &mqtt_password, &mqtt_topic};
}

// Getter methods
const std::string &ManageConfig::getSSID() const { return ssid; }
const std::string &ManageConfig::getPassword() const { return password; }
const std::string &ManageConfig::getMQTTServer() const { return mqtt_server; }
const std::string &ManageConfig::getMQTTPort() const { return mqtt_port; }
const std::string &ManageConfig::getMQTTUser() const { return mqtt_user; }
const std::string &ManageConfig::getMQTTPassword() const { return mqtt_password; }
const std::string &ManageConfig::getMQTTTopic() const { return mqtt_topic; }

// Setter methods
void ManageConfig::setSSID(const std::string &value) { ssid = value; }
void ManageConfig::setPassword(const std::string &value) { password = value; }
void ManageConfig::setMQTTServer(const std::string &value) { mqtt_server = value; }
void ManageConfig::setMQTTPort(const std::string &value) { mqtt_port = value; }
void ManageConfig::setMQTTUser(const std::string &value) { mqtt_user = value; }
void ManageConfig::setMQTTPassword(const std::string &value) { mqtt_password = value; }
void ManageConfig::setMQTTTopic(const std::string &value) { mqtt_topic = value; }

// formatString method implementation

std::vector<std::string> ManageConfig::formatString(const std::string &str, char delim)
{
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    // Splitting the string by the delimiter
    while (std::getline(ss, token, delim))
    {
        // Replace spaces with "//"
        size_t pos = 0;
        while ((pos = token.find(' ', pos)) != std::string::npos)
        {
            token.replace(pos, 1, "_");
            pos += 2; // Move past the "//" we just inserted
        }
        tokens.push_back(token);
    }

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
