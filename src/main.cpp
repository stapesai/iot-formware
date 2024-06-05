// Path: src/main.cpp

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
bool isConfigured = false;

Utils utils;                                           // Utility class to handle common functions
NVSManager config;                                     // Nvs manager class
WiFiClient wifiClient;                                 // WiFi client for MQTT
PubSubClient mqttclient(wifiClient);                   // MQTT client
MQTTManager mqttManager(&mqttclient, &config, &utils); // MQTT manager
BLEConfigCallbacks bleConfigCallbacks;                 // BLE Config Callbacks

void setup()
{
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Serial Initialized");

  // Initialize Buzzer and Reset LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(RESET_LED, OUTPUT);
  pinMode(NVS_RESET_PIN, INPUT);

  // Attach interrupt to reset the NVS
  attachInterrupt(NVS_RESET_PIN, config.resetNVS, FALLING);

  // Initialize NVS
  config.intialiseNVS();

  // Create a unique device name by prefixing with a string
  Serial.print("Device Name: ");
  Serial.println(utils.generateName());

  // Check if the device is configured
  isConfigured = config.isNVSEmpty() ? false : true;

  if (!isConfigured)
  {
    // Initialize BLE if the device is not configured
    bleConfigCallbacks.initialiseBLE(utils);
    Serial.println("Data is not present in NVS");
    Serial.println("Waiting for a client connection to notify...");
    Serial.println("The device started, now you can pair it with BLE Scanner!");
  }
  else
  {
    // Read data from NVS and store it in the config object
    config.readConfig();
  }
}

// TODO: Fix Reconnection

void loop()
{
  unsigned long currentMillis = millis();

  if (isConfigured)
  {
    // Check if the device is connected to WiFi
    if (WiFi.status() == WL_CONNECTED)
    {
      // Check if the device is connected to MQTT
      if (mqttclient.connected())
      {
        // Check if the device is connected to MQTT
        if (currentMillis - previousMillisMQTT >= keepAliveInterval)
        {
          previousMillisMQTT = currentMillis;
          Serial.println("Publishing keep alive message...");
          mqttManager.publish_keep_alive_message(keep_alive_counter);
          keep_alive_counter++;
        }
      }
      else
      {
        // Reconnect to MQTT
        if (currentMillis - previousMillisMQTT >= requiredInterval)
        {
          previousMillisMQTT = currentMillis;
          keep_alive_counter = 0;
          Serial.println("Reconnecting to MQTT...");
          mqttManager.connectToMQTT();
        }
      }
    }
    else
    {
      // Reconnect to WiFi
      if (currentMillis - previousMillisWiFi >= requiredInterval)
      {
        previousMillisWiFi = currentMillis;
        keep_alive_counter = 0;
        Serial.println("Reconnecting to WiFi...");
        utils.initialiseWiFi(&config);
      }
    }
  }
  else
  {
    if (currentMillis - previousMillisWiFi >= requiredInterval)
    {
      previousMillisWiFi = currentMillis;
      Serial.println("Waiting for a client to upload configuration...");
    }
  }
}