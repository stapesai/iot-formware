#include "BLEConfigCallbacks.h"
#include <Arduino.h>
#include <esp_system.h>
#include <iostream>

BLEConfigCallbacks::BLEConfigCallbacks(ManageConfig &config) : config(config) {}

void BLEConfigCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    // Parse the characteristic value
    std::string value = pCharacteristic->getValue();
    char delim = ';';
    std::vector<char *> tokens = config.formatString(value.c_str(), delim); // Ensure formatString takes const char* and returns vector<char*>

    if (tokens.size() == 7)
    {
        config.setWiFiSSID(tokens[0]);
        std::cout << "SSID: " << config.getWiFiSSID() << std::endl;

        config.setWiFiPassword(tokens[1]);
        std::cout << "Password: " << config.getWiFiPassword() << std::endl;

        config.setMQTTServer(tokens[2]);
        std::cout << "MQTT Server: " << config.getMQTTServer() << std::endl;

        config.setMQTTPort(tokens[3]);
        std::cout << "MQTT Port: " << config.getMQTTPort() << std::endl;

        config.setMQTTUser(tokens[4]);
        std::cout << "MQTT User: " << config.getMQTTUser() << std::endl;

        config.setMQTTPassword(tokens[5]);
        std::cout << "MQTT Password: " << config.getMQTTPassword() << std::endl;

        config.setMQTTTopic(tokens[6]);
        std::cout << "MQTT Topic: " << config.getMQTTTopic() << std::endl;

        // Write configuration to NVS
        config.writeConfig();
        Serial.println("Data written to NVS");

        // Restart the ESP32
        Serial.println("Restarting ESP32...");
        delay(1000);
        esp_restart();
    }
    else
    {
        Serial.println("Not enough tokens received.");
    }
}
