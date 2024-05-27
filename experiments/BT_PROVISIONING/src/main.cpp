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

// Define UUIDs
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

Preferences preferences;

struct Config
{
  char ssid[32];
  char password[32];
  char mqtt_server[32];
  char mqtt_port[6];
  char mqtt_user[32];
  char mqtt_password[32];
  char mqtt_topic[32];
  char *fields[7] = {ssid, password, mqtt_server, mqtt_port,
                     mqtt_user, mqtt_password, mqtt_topic};
} config;

void writeConfig()
{
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    Serial.println("Error (opening NVS handle)");
  }
  else
  {
    for (int i = 0; i < 7; ++i)
    {
      err = nvs_set_str(my_handle, config.fields[i], config.fields[i]);
      if (err != ESP_OK)
      {
        Serial.println("Error (writing to NVS)");
      }
    }
    nvs_commit(my_handle);
    nvs_close(my_handle);
  }
}

void readConfig()
{
  nvs_handle_t my_handle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
  if (err != ESP_OK)
  {
    Serial.println("Error (opening NVS handle)");
  }
  else
  {
    for (int i = 0; i < 7; ++i)
    {
      size_t required_size;
      err = nvs_get_str(my_handle, config.fields[i], NULL, &required_size);
      if (err != ESP_OK)
      {
        Serial.println("Error (reading from NVS)");
      }
      else
      {
        char *value = (char *)malloc(required_size);
        err = nvs_get_str(my_handle, config.fields[i], value, &required_size);
        if (err != ESP_OK)
        {
          Serial.println("Error (reading from NVS)");
        }
        else
        {
          Serial.printf("Read from NVS: %s\n", value);
          // strncpy(config.fields[i], value, sizeof(config.fields[i]));
        }
        free(value);
      }
    }
    nvs_close(my_handle);
  }
}

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
    std::string str = value;
    std::vector<std::string> tokens = splitString(str, delim);

    if (tokens.size() == 7)
    {
      // for (int i = 0; i < 7; ++i)
      // {
      //   strncpy(config.fields[i], tokens[i].c_str(), sizeof(config.fields[i]));
      //   Serial.printf("Received by ble: %s\n", config.fields[i]);

      // }

      strncpy(config.ssid, tokens[0].c_str(), sizeof(config.ssid));
      strncpy(config.password, tokens[1].c_str(), sizeof(config.password));
      strncpy(config.mqtt_server, tokens[2].c_str(), sizeof(config.mqtt_server));
      strncpy(config.mqtt_port, tokens[3].c_str(), sizeof(config.mqtt_port));
      strncpy(config.mqtt_user, tokens[4].c_str(), sizeof(config.mqtt_user));
      strncpy(config.mqtt_password, tokens[5].c_str(), sizeof(config.mqtt_password));
      strncpy(config.mqtt_topic, tokens[6].c_str(), sizeof(config.mqtt_topic));
      writeConfig();
      Serial.println("Data written to NVS");
    }
    else
    {
      Serial.println("Not enough tokens received.");
    }
  }
};

// function to write and read config to nvs

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

  BLEDevice::init("PROV_123"); // Initialize BLE

  BLEServer *pServer = BLEDevice::createServer();              // Create BLE server
  BLEService *pService = pServer->createService(SERVICE_UUID); // Create BLE service
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE); // Create BLE characteristic

  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start(); // Start the service
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();

  readConfig();

  preferences.begin("storage", true);
  if (preferences.getBytesLength("ssid") > 0)
  {
    Serial.println("Data present in NVS");
    readConfig();

    // Stop BLE service and advertising
    pService->stop();
    pAdvertising->stop();

    Serial.println("BLE service stopped.");
  }
  else
  {
    Serial.println("Waiting for a client connection to notify...");
    Serial.println("The device started, now you can pair it with BLE Scanner!");
  }
}



void loop()

{
  digitalWrite(16, HIGH);
  delay(1000);
  digitalWrite(16, LOW);
  delay(1000);
  if (digitalRead(0) == LOW)
  {

    Serial.println("Resetting nvs");

    delay(5000);

    if (nvs_flash_erase() == ESP_OK)
    {
      Serial.println("Resetting nvs");
      Serial.println("Erased nvs");
    }
    else
    {
      Serial.println("Error erasing nvs");
    }
  }
}
