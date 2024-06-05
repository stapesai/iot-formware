// Path: src/WiFiManager.cpp

#include <WiFiManager.h>

// Constructor
WiFiManager::WiFiManager(NVSManager *config) : config(config) {}

// Initialize WiFi
void WiFiManager::initializeWiFi()
{
    Serial.print("Connecting to SSID: ");
    Serial.println(config->getWiFiSSID().c_str());
    Serial.print("With password: ");
    Serial.println(config->getWiFiPassword().c_str());

    WiFi.begin(config->getWiFiSSID().c_str(), config->getWiFiPassword().c_str());

    uint8_t attemptCount = 0;
    while (WiFi.status() != WL_CONNECTED && attemptCount < 10)
    {
        delay(1000);
        Serial.print(".");
        attemptCount++;
    }

    Serial.println();
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Connected to the WiFi network");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("Failed to connect to WiFi");
    }
}
