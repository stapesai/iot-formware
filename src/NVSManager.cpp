// Path: src/NVSManager.cpp

#include "NVSManager.h"
#include "Utils.h"
#include <Arduino.h>

// Constructor
NVSManager::NVSManager() {
  fields = {&wifi_ssid, &wifi_password, &mqtt_broker, &mqtt_port,
            &mqtt_user, &mqtt_password, &mqtt_topic};
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
void NVSManager::setWiFiSSID(const std::string &value) { wifi_ssid = value; }
void NVSManager::setWiFiPassword(const std::string &value) {
  wifi_password = value;
}
void NVSManager::setMQTTBroker(const std::string &value) {
  mqtt_broker = value;
}
void NVSManager::setMQTTPort(const std::string &value) { mqtt_port = value; }
void NVSManager::setMQTTUser(const std::string &value) { mqtt_user = value; }
void NVSManager::setMQTTPassword(const std::string &value) {
  mqtt_password = value;
}
void NVSManager::setMQTTTopic(const std::string &value) { mqtt_topic = value; }

// Interrupt handler to reset the module
void IRAM_ATTR NVSManager::resetNVS() {
  if (digitalRead(NVS_RESET_PIN) == LOW) {
    if (nvs_flash_erase() == ESP_OK) {
      Serial.println("NVS erased");
    } else {
      Serial.println("Error erasing NVS");
    }

    Serial.println("Restarting ESP32...");
    delay(1000);
    esp_restart();
  }
}

bool NVSManager::isNVSEmpty() {
  err = nvs_open("storage", NVS_READWRITE, &nvs_handler);
  // use all available keys to check if any of them doesn't exists
  const char *key = "wifi_ssid";

  // Check NVS stats
  size_t required_size;
  err = nvs_get_str(nvs_handler, key, NULL, &required_size);
  nvs_close(nvs_handler);

  if (err == ESP_OK) {
    Serial.printf("Key: %s\n", key);
    return false;
  } else if (err == ESP_ERR_NVS_NOT_FOUND) {
    return true;
  } else {
    Serial.printf("Error getting key '%s': %s\n", key, esp_err_to_name(err));
    return true;
  }
}

void NVSManager::intialiseNVS() {
  err = nvs_flash_init();

  // Check if initialization encountered specific errors
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // If NVS initialization encountered errors related to free pages or new
    // version found, erase the entire NVS partition to resolve these issues
    ESP_ERROR_CHECK(nvs_flash_erase());
    // Re-initialize NVS after erasing the partition
    err = nvs_flash_init();
  }

  ESP_ERROR_CHECK(err);
}

void NVSManager::writeConfig() {
  if (nvs_open("storage", NVS_READWRITE, &nvs_handler) != ESP_OK) {
    Serial.println("Error opening NVS handle");
    return;
  }

  for (int i = 0; i < 7; ++i) {
    Serial.printf("Writing to NVS: %s = %s\n", keys[i], fields[i]->c_str());
    if (nvs_set_str(nvs_handler, keys[i], fields[i]->c_str()) != ESP_OK) {
      Serial.printf("Error writing to NVS for key '%s'\n", keys[i]);
    }
  }

  if (nvs_commit(nvs_handler) != ESP_OK) {
    Serial.println("Error committing NVS changes");
  } else {
    Serial.println("NVS committed successfully");
  }
  nvs_close(nvs_handler);
}

void NVSManager::readConfig() {

  err = nvs_open("storage", NVS_READWRITE, &nvs_handler);

  if (err != ESP_OK) {
    Serial.println("Error opening NVS handle");
    return;
  } else {
    Serial.println("NVS opened successfully");
  }

  for (int i = 0; i < 7; ++i) {
    size_t required_size;
    err = nvs_get_str(nvs_handler, keys[i], NULL, &required_size);
    if (err != ESP_OK) {
      // Serial.printf("Error reading size for key '%s'\n", keys[i]);
      continue;
    }

    char *value = (char *)malloc(required_size);
    if (value == nullptr) {
      Serial.println("Failed to allocate memory for NVS read");
      continue;
    }

    err = nvs_get_str(nvs_handler, keys[i], value, &required_size);
    if (err == ESP_OK) {
      *fields[i] = std::string(value);
      Serial.printf("Read from NVS: %s = %s\n", keys[i], value);
    } else {
      Serial.printf("Error reading value for key '%s'\n", keys[i]);
    }
    free(value);
  }
  nvs_close(nvs_handler);
}
