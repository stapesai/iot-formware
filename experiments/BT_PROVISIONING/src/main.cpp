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

class ManageConfig
{
private:
  std::string ssid;
  std::string password;
  std::string mqtt_server;
  std::string mqtt_port;
  std::string mqtt_user;
  std::string mqtt_password;
  std::string mqtt_topic;
  std::array<std::string *, 7> fields;
  const char *keys[7] = {"ssid", "password", "mqtt_server", "mqtt_port", "mqtt_user", "mqtt_password", "mqtt_topic"};

public:
  ManageConfig() { fields = {&ssid, &password, &mqtt_server, &mqtt_port, &mqtt_user, &mqtt_password, &mqtt_topic}; }

  const std::string &getSSID() const { return ssid; }
  const std::string &getPassword() const { return password; }
  const std::string &getMQTTServer() const { return mqtt_server; }
  const std::string &getMQTTPort() const { return mqtt_port; }
  const std::string &getMQTTUser() const { return mqtt_user; }
  const std::string &getMQTTPassword() const { return mqtt_password; }
  const std::string &getMQTTTopic() const { return mqtt_topic; }

  void setSSID(const std::string &value) { ssid = value; }
  void setPassword(const std::string &value) { password = value; }
  void setMQTTServer(const std::string &value) { mqtt_server = value; }
  void setMQTTPort(const std::string &value) { mqtt_port = value; }
  void setMQTTUser(const std::string &value) { mqtt_user = value; }
  void setMQTTPassword(const std::string &value) { mqtt_password = value; }
  void setMQTTTopic(const std::string &value) { mqtt_topic = value; }

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
        err = nvs_set_str(my_handle, keys[i], fields[i]->c_str());
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

  void readConfig()
  {
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READONLY, &my_handle);
    if (err != ESP_OK)
    {
      Serial.println("Error opening NVS handle");
    }
    else
    {
      for (int i = 0; i < 7; ++i)
      {
        size_t required_size;
        // Get the size of the configuration field
        err = nvs_get_str(my_handle, keys[i], NULL, &required_size);
        if (err != ESP_OK)
        {
          Serial.printf("Error reading size from NVS for field %d (%s)\n", i, keys[i]);
        }
        else
        {
          // Allocate memory to store the configuration field
          char *value = (char *)malloc(required_size);
          // Read the configuration field from NVS
          err = nvs_get_str(my_handle, keys[i], value, &required_size);
          if (err != ESP_OK)
          {
            Serial.printf("Error reading value from NVS for field %d (%s)\n", i, keys[i]);
          }
          else
          {
            // Copy the value to the configuration field
            *fields[i] = std::string(value); // Convert char* to std::string
            std::cout << "Read from NVS: " << keys[i] << " = " << *(fields[i]) << std::endl;
          }
          free(value); // Free the allocated memory
        }
      }
      nvs_close(my_handle);
    }
  }
  // Method to print all fields
  void printFields() const
  {
    for (const auto &field : fields)
    {
      std::cout << *field << std::endl;
    }
  }
};
ManageConfig config;

class MyCallbacks : public BLECharacteristicCallbacks,
                    public ManageConfig
{
public:
  void onWrite(BLECharacteristic *pCharacteristic)
  {
    char delim = ';';
    std::string value = pCharacteristic->getValue();
    std::vector<std::string> tokens = config.splitString(value, delim);

    if (tokens.size() == 7)
    {
      for (int i = 0; i < 7; ++i)
      {
        switch (i)
        {
        case 0:
          config.setSSID(tokens[i]);
          std::cout << "SSID: " << config.getSSID() << std::endl;
          break;
        case 1:
          config.setPassword(tokens[i]);
          std::cout << "Password: " << config.getPassword() << std::endl;
          break;
        case 2:
          config.setMQTTServer(tokens[i]);
          std::cout << "MQTT Server: " << config.getMQTTServer() << std::endl;
          break;
        case 3:
          config.setMQTTPort(tokens[i]);
          std::cout << "MQTT Port: " << config.getMQTTPort() << std::endl;

          break;
        case 4:
          config.setMQTTUser(tokens[i]);
          std::cout << "MQTT User: " << config.getMQTTUser() << std::endl;
          break;
        case 5:
          config.setMQTTPassword(tokens[i]);
          std::cout << "MQTT Password: " << config.getMQTTPassword() << std::endl;
          break;
        case 6:
          config.setMQTTTopic(tokens[i]);
          std::cout << "MQTT Topic: " << config.getMQTTTopic() << std::endl;
          break;
        default:
          break;
        }
      }

      // Write the configuration to NVS
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

  // readConfig();

  if (strlen(config.getSSID().c_str()) > 0)
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
}

void loop()
{

  if (digitalRead(0) == LOW)
  {
    unsigned long startTime = millis(); // Record the start time

    Serial.println("Resetting NVS");

    while (millis() - startTime < 5000)
    {
      digitalWrite(16, HIGH);
      delay(100);
      digitalWrite(16, LOW);
      delay(100);
    }
    delay(1000);

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
