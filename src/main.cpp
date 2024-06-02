// Path: src/main.cpp
// Main file that contains the setup and loop functions.

// Include the necessary libraries
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <nvs_flash.h>
#include <utils.h>
#include <PubSubClient.h>
#include <BLEConfigCallbacks.h>
#include <MQTTManager.h>

unsigned long previousMillis = 0;
unsigned long keep_alive_counter = 1;

Utils utils;                                           // Utility class to handle common functions
ManageConfig config;                                   // Configuration manager class
WiFiClient wifiClient;                                 // WiFi client for MQTT
PubSubClient mqttclient(wifiClient);                   // MQTT client
MQTTManager mqttManager(&mqttclient, &config, &utils); // MQTT manager

// Interrupt handler to reset the module
void IRAM_ATTR resetNVS()
{
  const unsigned long LONG_PRESS_DURATION = 5000; // 5 seconds in milliseconds
  static unsigned long buttonPressStartTime = 0;  // Static to retain value between function calls

  if (digitalRead(NVS_RESET_PIN) == LOW)
  {
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

void setup()
{
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Serial Initialized");

  // Initialize Buzzer and Reset LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(RESET_LED, OUTPUT);
  pinMode(NVS_RESET_PIN, INPUT);

  // Initialize NVS flash storage and store the result in 'err'
  esp_err_t err = nvs_flash_init();

  // Check if initialization encountered specific errors
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // If NVS initialization encountered errors related to free pages or new version found,
    // erase the entire NVS partition to resolve these issues
    ESP_ERROR_CHECK(nvs_flash_erase());

    // Re-initialize NVS after erasing the partition
    err = nvs_flash_init();
  }

  // Check if NVS initialization was successful after potential erasure and re-initialization
  ESP_ERROR_CHECK(err);

  // Create a unique device name by prefixing with a string

  Serial.print("Device Name: ");
  Serial.println(utils.generateName());
  attachInterrupt(digitalPinToInterrupt(NVS_RESET_PIN), resetNVS, FALLING);

  // Initialize BLE with the unique device name
  BLEDevice::init(utils.generateName());

  // TODO: Add BLE security, Make device only visible to `JarvisHome` app.
  // Create BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
  pCharacteristic->setCallbacks(new BLEConfigCallbacks(config));
  pService->start();

  // Set up advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06); // Functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  // BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);



  // Read the configuration from NVS, if it exists
  config.readConfig();

  // Check if the SSID is empty
  // TODO: This is a temporary solution to check if the SSID is empty, and should be replaced with a better solution
  if (config.getWiFiSSID().empty())
  {
    Serial.println("Waiting for a client connection to notify...");
    Serial.println("The device started, now you can pair it with BLE Scanner!");
  }
  else
  {
    Serial.println("Data is present in NVS");
    pService->stop();
    pAdvertising->stop();
    BLEDevice::deinit(true);
    Serial.println("BLE service stopped.");
    utils.initialiseWiFi(&config);
    mqttManager.connectToMQTT();
  }
}

void loop()
{
  unsigned long currentMillis = millis();
  if (!config.getWiFiSSID().empty())
  {
    while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis >= 5000)
    {
      Serial.println("Connecting to Wifi...");
      previousMillis = currentMillis;
      utils.statusBuzzer(STATUS_BUZZER, 100);
      utils.initialiseWiFi(&config);
      if (!mqttclient.connected())
      {
        Serial.println("MQTT Connection lost.. Reconnecting..");
        mqttManager.connectToMQTT();
      }// Keep alive message
      else if (currentMillis - previousMillis >= 50000 && mqttclient.connected())
      {
        Serial.println("Publishing keep alive message");
        mqttManager.publish_keep_alive_message(keep_alive_counter);
        keep_alive_counter++;
      }
    }
  }
}