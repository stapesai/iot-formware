// Default header files
#include <Arduino.h>
#include <ArduinoJson.h>
#include <utils.h>
#include <MQTTManager.h>
#include <WiFiProvisioning.h>

// MQTT Client

void setup()
{

  Serial.begin(115200);
  log_d("Serial Initialized");

  // Initialize Buzzer and In-Built LED
  pinMode(STATUS_BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  // Initialize Shift Register
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  updateShiftRegister();
  log_d("Shift Register Initialized");
  WiFi.onEvent(SysProvEvent);
  if (CONFIG_BLUEDROID_ENABLED)
  {
    Serial.println("Begin Provisioning using BLE");
    uint8_t uuid[16] = {0xb4, 0xdf, 0x5a, 0x1c, 0x3f, 0x6b, 0xf4, 0xbf, 0xea, 0x4a, 0x82, 0x03, 0x04, 0x90, 0x1a, 0x02};
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name, service_key, uuid, reset_provisioned);
  }



}
void loop()
{
  setOutput(2);
  delay(1000);
  clearOutput(2);
  delay(1000);
  setOutput(3);
  delay(1000);
  clearOutput(3);
  delay(1000);
  if (digitalRead(0) == LOW)
  {
    Serial.println("Reset Button Pressed");
    delay(5000);

    if (digitalRead(0) == LOW)
    {
      Serial.println("Resetting");
      wifi_prov_mgr_reset_provisioning();
      ESP.restart();
    }
  }
}
