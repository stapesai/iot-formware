// Path: src/BLEConfigCallbacks.cpp

#include "BLEConfigCallbacks.h"
#include <Arduino.h>
#include <esp_system.h>
#include <iostream>

BLEConfigCallbacks::BLEConfigCallbacks(ManageConfig &config) : config(config) {}

void BLEConfigCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    char delim = ';';
    std::string value = pCharacteristic->getValue();
    std::vector<std::string> tokens = config.formatString(value, delim);

    if (tokens.size() == 7)
    {
        config.setSSID(tokens[0]);
        std::cout << "SSID: " << config.getSSID() << std::endl;

        config.setPassword(tokens[1]);
        std::cout << "Password: " << config.getPassword() << std::endl;

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

        config.writeConfig();
        Serial.println("Data written to NVS");

        Serial.println("Restarting ESP32...");
        delay(1000);
        esp_restart();
    }
    else
    {
        Serial.println("Not enough tokens received.");
    }
}
