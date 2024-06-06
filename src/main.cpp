// Path: src/main.cpp

#include <Arduino.h>
#include <BLEManager.h>
#include <MQTTManager.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

class MainApp {
public:
  MainApp();
  void setup();
  void loop();

private:
  void setupPins();
  void setupNVS();
  void setupBLE();
  void setupMQTT();
  void checkWiFiAndMQTT(unsigned long);

  unsigned long previousMillisWiFi;
  unsigned long previousMillisMQTT;
  unsigned long previousMillisKeepAlive;
  unsigned long keepAliveInterval;
  unsigned long requiredInterval;
  unsigned long keep_alive_counter;
  bool isConfigured;

  Utils utils;                   // Utility class to handle common functions
  NVSManager config;             // NVS manager class
  WiFiClient wifiClient;         // WiFi client for MQTT
  PubSubClient mqttClient;       // MQTT client
  MQTTManager mqttManager;       // MQTT manager
  BLEConfigCallbacks bleManager; // BLE Config Callbacks
  WiFiManager wifiManager;       // WiFi Manager
};

MainApp::MainApp()
    : previousMillisWiFi(0), previousMillisMQTT(0), keepAliveInterval(50000),
      requiredInterval(5000), keep_alive_counter(1), isConfigured(false),
      mqttClient(wifiClient), mqttManager(&mqttClient, &config, &utils),
      bleManager(config, utils), wifiManager(&config) {}

void MainApp::setup() {
  Serial.begin(115200);
  setupPins();
  setupNVS();

  if (isConfigured) {
    config.readConfig();
    setupMQTT();
  } else {
    setupBLE();
  }
}

void MainApp::setupPins() {
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(RESET_LED, OUTPUT);
  pinMode(NVS_RESET_PIN, INPUT);
  attachInterrupt(NVS_RESET_PIN, config.resetNVS, FALLING);
}

void MainApp::setupNVS() {
  config.intialiseNVS();
  isConfigured = !config.isNVSEmpty();
}

void MainApp::setupBLE() {
  bleManager.initializeBLE();
  Serial.println("Data is not present in NVS");
  Serial.println("Waiting for a client connection to notify...");
  Serial.println("The device started, now you can pair it with BLE Scanner!");
}

void MainApp::setupMQTT() { mqttManager.initializeMQTT(); }

void MainApp::loop() {
  unsigned long currentMillis = millis();
  if (isConfigured) {
    checkWiFiAndMQTT(currentMillis);
  } else {
    if (currentMillis - previousMillisWiFi >= requiredInterval) {
      previousMillisWiFi = currentMillis;
      Serial.println("Waiting for a client to upload configuration...");
    }
  }
}

void MainApp::checkWiFiAndMQTT(unsigned long currentMillis) {
  if (WiFi.status() == WL_CONNECTED) {
    if (mqttClient.connected()) {
      if (currentMillis - previousMillisMQTT >= keepAliveInterval) {
        previousMillisMQTT = currentMillis;
        Serial.println("Publishing keep alive message...");
        mqttManager.publish_keep_alive_message(keep_alive_counter);
        keep_alive_counter++;
      }
    } else {
      if (currentMillis - previousMillisMQTT >= requiredInterval) {
        previousMillisMQTT = currentMillis;
        keep_alive_counter = 0;
        Serial.println("Reconnecting to MQTT...");
        mqttManager.connectToMQTT();
      }
    }
  } else {
    if (currentMillis - previousMillisWiFi >= requiredInterval) {
      previousMillisWiFi = currentMillis;
      keep_alive_counter = 0;
      Serial.println("Reconnecting to WiFi...");
      wifiManager.initialiseWiFi();
    }
  }
}

MainApp app;

void setup() { app.setup(); }

void loop() { app.loop(); }
