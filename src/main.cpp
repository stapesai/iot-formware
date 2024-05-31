// Path: src/main.cpp
#include <Arduino.h>
#include <utils.h>
#include <nvs_flash.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEConfigCallbacks.h>
#include <MQTTManager.h>

unsigned long previousMillis = 0;
unsigned long keep_alive_counter = 1;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

MQTTManager mqttManager;

void initaliseWiFi()
{
  Serial.print("Connecting to SSID: ");
  Serial.println(config.getSSID().c_str());
  Serial.print("With password: ");
  Serial.println(config.getPassword().c_str());

  WiFi.begin(config.getSSID().c_str(), config.getPassword().c_str());

  Serial.println();
  if (WiFi.isConnected() == WL_CONNECTED)
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

// create reset interrupt

void IRAM_ATTR resetModule()
{
  for (int i = 0; i < 25; i++)
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

void setup()
{

  Serial.begin(115200);
  log_d("Serial Initialized");

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  // Retrieve the MAC address

  Serial.print("Device MAC Address: ");
  Serial.println(macAddress);

  // Create a unique device name by prefixing with a string
  String deviceName = "Jarvis_" + macAddress;
  Serial.print("Device Name: ");
  Serial.println(deviceName);
  attachInterrupt(digitalPinToInterrupt(RESET_PIN), resetModule, FALLING);

  // // Initialize BLE with the unique device name
  BLEDevice::init(deviceName.c_str());

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

  config.readConfig();
  if (!config.getSSID().empty())
  {
    Serial.println("Data present in NVS");
    pService->stop();
    pAdvertising->stop();
    BLEDevice::deinit(true);
    Serial.println("BLE service stopped.");
    initaliseWiFi();
    mqttManager.connectToMQTT();
  }
  else
  {
    Serial.println("Waiting for a client connection to notify...");
    Serial.println("The device started, now you can pair it with BLE Scanner!");
  }

  // Initialize Buzzer and In-Built LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(RESET_LED, OUTPUT);
}
void loop()
{
  unsigned long currentMillis = millis();
  // WiFi
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Connecting to Wifi...");
    previousMillis = currentMillis;
    statusBuzzer(2, 100);
    initaliseWiFi();
  }
  if (currentMillis - previousMillis >= 50000)
  {
    previousMillis = currentMillis;
    if (mqttclient.connected())
    {
      Serial.println("Publishing keep alive message");
      mqttManager.publish_keep_alive_message(keep_alive_counter);
      keep_alive_counter++;
    }
    else
    {
      Serial.println("-----------------------");
      Serial.println("MQTT Connection lost.. Reconnecting..");
      mqttManager.connectToMQTT();
    }
  }
}
