#include "BLEConfigCallbacks.h"
#include <Arduino.h>
#include <esp_system.h>
#include <iostream>

BLEConfigCallbacks::BLEConfigCallbacks(ManageConfig &config) : config(config) {}

void BLEConfigCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{

    std::string value = pCharacteristic->getValue();
    std::vector<std::string> tokens = config.formatString(value, delim );

    if (tokens.size() == 7)
    {
        config.setWiFiSSID(tokens[0]);
        config.setWiFiPassword(tokens[1]);
        config.setMQTTBroker(tokens[2]);
        config.setMQTTPort(tokens[3]);
        config.setMQTTUser(tokens[4]);
        config.setMQTTPassword(tokens[5]);
        config.setMQTTTopic(tokens[6]);
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
