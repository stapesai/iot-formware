// Path: src/main.cpp
// Main file that contains the setup and loop functions.

// Include the necessary libraries

// TODO: Add BLE security, Make device only visible to `JarvisHome` app.

#include <Arduino.h>
#include <PubSubClient.h>
#include <BLEManager.h>
#include <MQTTManager.h>

unsigned long previousMillisWiFi = 0;
unsigned long previousMillisMQTT = 0;
unsigned long previousMillisKeepAlive = 0;
unsigned long keepAliveInterval = 50000; // 50 seconds
unsigned long requiredInterval = 5000;   // 5 seconds
unsigned long keep_alive_counter = 1;

Utils utils;                                           // Utility class to handle common functions
NVSManager config;                                     // Nvs manager class
WiFiClient wifiClient;                                 // WiFi client for MQTT
PubSubClient mqttclient(wifiClient);                   // MQTT client
MQTTManager mqttManager(&mqttclient, &config, &utils); // MQTT manager
BLEConfigCallbacks bleConfigCallbacks(config, utils);
BLEServer *pServer;
BLEService *pService;
BLECharacteristic *pCharacteristic;
BLEAdvertising *pAdvertising; // BLE Config Callbacks

void setup()
{
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Serial Initialized");
  attachInterrupt(NVS_RESET_PIN, config.resetNVS, FALLING);

  // Initialize Buzzer and Reset LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(RESET_LED, OUTPUT);
  pinMode(NVS_RESET_PIN, INPUT);

  config.intialiseNVS();

  // Create a unique device name by prefixing with a string

  Serial.print("Device Name: ");
  Serial.println(utils.generateName());


  if (config.isNVSEmpty())
  {
    bleConfigCallbacks.initialiseBLE();
    config.readConfig();
    Serial.println("Data is not present in NVS");
    Serial.println("Waiting for a client connection to notify...");
    Serial.println("The device started, now you can pair it with BLE Scanner!");
  }
  else
  {
    bleConfigCallbacks.deinitaliseBLE();
    Serial.println("BLE service stopped.");
    config.readConfig();
    utils.initialiseWiFi(&config);
    // mqttManager.connectToMQTT();
  }
}

// TODO: Fix Reconnection

void loop()
{
  unsigned long currentMillis = millis();
  // Check if the device is connected to WiFi

  if (!config.isNVSEmpty())
  {
    
    while (WiFi.status() == WL_DISCONNECTED)
    {
      Serial.println("Reconnecting to WiFi...");
      utils.initialiseWiFi(&config);
      delay(1000);

      
    }

    // Check if the device is connected to MQTT

    while (!mqttclient.connected())
    {
      Serial.println("Reconnecting to MQTT...");
      mqttManager.connectToMQTT();
      delay(1000);
    }
   
    
  }
  else
  {
    log_d("Device is not configured yet.");
  }

 
}
