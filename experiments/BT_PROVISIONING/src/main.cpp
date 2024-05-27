#include <iostream>
#include <string.h>
#include <sstream>
#include <Arduino.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Preferences.h>
#include <nvs_flash.h>
#include <esp_system.h> // Include the header for restarting

// Define UUIDs for BLE service and characteristic
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

Preferences preferences;

// Define a structure to hold configuration data
struct Config
{
  char ssid[32];
  char password[32];
  char mqtt_server[32];
  char mqtt_port[6];
  char mqtt_user[32];
  char mqtt_password[32];
  char mqtt_topic[32];
  char *fields[7] = {ssid, password, mqtt_server, mqtt_port, mqtt_user, mqtt_password, mqtt_topic};
} config;

const char *keys[7] = {"ssid", "password", "mqtt_server", "mqtt_port", "mqtt_user", "mqtt_password", "mqtt_topic"};

// Function to write configuration data to NVS
void writeConfig()
{
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    Serial.println("Error opening NVS handle");
  }
  else
  {
    for (int i = 0; i < 7; ++i)
    {
      // Write the configuration field to NVS
      err = nvs_set_str(my_handle, keys[i], config.fields[i]);
      if (err != ESP_OK)
      {
        Serial.printf("Error writing to NVS for field %d (%s)\n", i, keys[i]);
      }
    }
    // Commit the changes to NVS
    nvs_commit(my_handle);
    nvs_close(my_handle);
  }
}

// Function to read configuration data from NVS
void readConfig()
{
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    Serial.println("Error opening NVS handle");
  }
  else
  {
    for (int i = 0; i < 7; ++i)
    {
      size_t required_size;
      err = nvs_get_str(my_handle, keys[i], NULL, &required_size);
      if (err != ESP_OK)
      {
        Serial.printf("Error reading size from NVS for field %d (%s)\n", i, keys[i]);
      }
      else
      {
        err = nvs_get_str(my_handle, keys[i], config.fields[i], &required_size);
        if (err != ESP_OK)
        {
          Serial.printf("Error reading value from NVS for field %d (%s)\n", i, keys[i]);
        }
        else
        {
          Serial.printf("Read from NVS: %s = %s\n", keys[i], config.fields[i]);
        }
      }
    }
    nvs_close(my_handle);
  }
}

// Helper function to split a string into tokens based on a delimiter
std::vector<std::string> splitString(const std::string &str, char delim)
{
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(str);
  while (std::getline(tokenStream, token, delim))
  {
    tokens.push_back(token);
  }
  return tokens;
}

// Callbacks for BLE characteristic
class MyCallbacks : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    char delim = ';';
    std::string value = pCharacteristic->getValue();
    std::vector<std::string> tokens = splitString(value, delim);

    if (tokens.size() == 7)
    {
      for (int i = 0; i < 7; ++i)
      {
        // Copy the received string into the configuration field
        strncpy(config.fields[i], tokens[i].c_str(), sizeof(config.fields[i]) - 1);
        // Ensure null termination
        config.fields[i][sizeof(config.fields[i]) - 1] = '\0';
        Serial.printf("Received by BLE: %s\n", config.fields[i]);
      }

      // Write the configuration to NVS
      writeConfig();
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
};

void setup()
{
  Serial.begin(115200);

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  BLEDevice::init("PROV_123");

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  readConfig();

  if (strlen(config.ssid) > 0)
  {
    Serial.println("Data present in NVS");

    pService->stop();
    pAdvertising->stop();

    Serial.println("BLE service stopped.");
  }
  else
  {
    Serial.println("Waiting for a client connection to notify...");
    Serial.println("The device started, now you can pair it with BLE Scanner!");
  }

  pinMode(16, OUTPUT);
  pinMode(0, INPUT_PULLUP);
}

void loop()
{
  digitalWrite(16, HIGH);
  delay(1000);
  digitalWrite(16, LOW);
  delay(1000);

  if (digitalRead(0) == LOW)
  {
    Serial.println("Resetting NVS");
    delay(5000);

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
