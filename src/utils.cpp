// Path: src/utils.h
// Description: This file contains the declaration of the Utils class.

#include <Arduino.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <utils.h>

const char* Utils::generateName()
{
    std::string result;
    std::string macAddress = WiFi.macAddress().c_str();

    // Replace ':' with '_'
    std::replace(macAddress.begin(), macAddress.end(), ':', '_');

    // Create the result string
    result = BLE_DEVICE_NAME + macAddress;

    // Return the C-style string
    return result.c_str();
}

void Utils::statusBuzzer(int times, int delayTime)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(STATUS_BUZZER, HIGH);
    delay(delayTime);
    digitalWrite(STATUS_BUZZER, LOW);
    delay(delayTime);
  }
}

bool Utils::isNVSEmpty() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        Serial.println("Error initializing NVS");
        return false;
    }

    // Check NVS stats
    nvs_stats_t nvs_stats;
    err = nvs_get_stats(NULL, &nvs_stats);
    if (err != ESP_OK) {
        Serial.println("Error getting NVS stats");
        return false;
    }

    // Check if used entries are zero
    return nvs_stats.used_entries == 0;
}


void Utils::initialiseWiFi(ManageConfig *config)
{
  Serial.print("Connecting to SSID: ");
  Serial.println(config->getWiFiSSID().c_str());
  Serial.print("With password: ");
  Serial.println(config->getWiFiPassword().c_str());

  WiFi.begin(config->getWiFiSSID().c_str(), config->getWiFiPassword().c_str());

  u_int8_t attemptCount = 0;
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