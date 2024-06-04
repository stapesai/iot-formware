// Path: src/main.cpp
// Main file that contains the setup and loop functions.

// Include the necessary libraries
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <nvs_flash.h>
#include <PubSubClient.h>
#include <BLEManager.h>
#include <MQTTManager.h>

unsigned long previousMillisWiFi = 0;
unsigned long previousMillisMQTT = 0;
unsigned long previousMillisKeepAlive = 0;
unsigned long keepAliveInterval = 50000; // 50 seconds
unsigned long wifiCheckInterval = 5000;  // 5 seconds
unsigned long keep_alive_counter = 1;

Utils utils;                                           // Utility class to handle common functions
NVSManager config;                                     // Nvs manager class
WiFiClient wifiClient;                                 // WiFi client for MQTT
PubSubClient mqttclient(wifiClient);                   // MQTT client
MQTTManager mqttManager(&mqttclient, &config, &utils); // MQTT manager
BLEConfigCallbacks bleConfigCallbacks(config, utils);  // BLE Config Callbacks

void setup()
{
  // Initialize Serial
  Serial.begin(115200);
  Serial.println("Serial Initialized");

  // Initialize Buzzer and Reset LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(RESET_LED, OUTPUT);
  pinMode(NVS_RESET_PIN, INPUT);

  config.intialiseNVS();

  // Create a unique device name by prefixing with a string

  Serial.print("Device Name: ");
  Serial.println(utils.generateName());

  // TODO: Add BLE security, Make device only visible to `JarvisHome` app.
  // TODO: If not enough tokens are received then it shoudn't stop the BLE server
  // TODO: Send the messages through BLE

  // Read the configuration from NVS, if it exists

  if (config.isNVSEmpty())
  {
    Serial.println("Data is not present in NVS");
    bleConfigCallbacks.initialiseBLE();
    Serial.println("Waiting for a client connection to notify...");
    Serial.println("The device started, now you can pair it with BLE Scanner!");
  }
  else
  {
    Serial.println("Data is present in NVS");
    bleConfigCallbacks.deinitaliseBLE();
    config.readConfig();
    Serial.println("BLE service stopped.");
    utils.initialiseWiFi(&config);
    mqttManager.connectToMQTT();
  }
}

// TODO: Fix Reconnection

void loop()
{
  unsigned long currentMillis = millis();

  config.resetNVS();

  if (currentMillis - previousMillisWiFi >= wifiCheckInterval)
  {
    previousMillisWiFi = currentMillis;

    if (!config.isNVSEmpty() == 0)
    {
      if (WiFi.status() != WL_CONNECTED)
      {
        Serial.println("Connecting to WiFi...");
        utils.statusBuzzer(STATUS_BUZZER, 100);
        utils.initialiseWiFi(&config);
      }
    }
  }
}

// // Check MQTT connection if WiFi is connected
// if (WiFi.status() == WL_CONNECTED)
// {
//   // Handle different MQTT states
//   switch (mqttclient.state())
//   {
//   case MQTT_CONNECTION_LOST:
//     Serial.println("MQTT Connection lost.. Reconnecting..");
//     mqttManager.connectToMQTT();
//     break;

//   case MQTT_DISCONNECTED:
//     Serial.println("MQTT Disconnected.. Reconnecting..");
//     mqttManager.connectToMQTT();
//     break;

//   case MQTT_CONNECTED:
//     // Send keep-alive message every 50 seconds
//     if (currentMillis - previousMillisKeepAlive >= keepAliveInterval)
//     {
//       Serial.println("Publishing keep alive message");
//       mqttManager.publish_keep_alive_message(keep_alive_counter);
//       keep_alive_counter++;
//       previousMillisKeepAlive = currentMillis;
//     }
//     break;

//   default:
//     Serial.println("done");
//     break;
//   }
