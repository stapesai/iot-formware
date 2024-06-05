// Path: src/utils.h
// Description: This file contains the declaration of the Utils class.

#include <Arduino.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <utils.h>

const char *Utils::generateName()
{
  std::string result;
  std::string macAddress = WiFi.macAddress().c_str();

  // Replace ':' with '_'
  std::replace(macAddress.begin(), macAddress.end(), ':', '_');

  // Create the result string
  result = BLE_DEVICE_NAME + macAddress;

  // Return the C-style string
  return result.c_str();
}
// Format incoming input string
std::vector<std::string> Utils::formatString(const std::string &str, char delim)
{
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delim))
  {
    tokens.push_back(token);
  }
  return tokens;
}

void Utils::statusBuzzer(int times, int delayTime)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(STATUS_BUZZER, HIGH);
    delay(delayTime);
    digitalWrite(STATUS_BUZZER, LOW);
    delay(delayTime);
  }
}

void Utils::initialiseWiFi(NVSManager *config)
{
  Serial.print("Connecting to SSID: ");
  Serial.println(config->getWiFiSSID().c_str());
  Serial.print("With password: ");
  Serial.println(config->getWiFiPassword().c_str());

  WiFi.begin(config->getWiFiSSID().c_str(), config->getWiFiPassword().c_str());

  u_int8_t attemptCount = 0;
  while (WiFi.status() != WL_CONNECTED && attemptCount < 10)
  {
    delay(1000);
    Serial.print(".");
    attemptCount++;
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED)
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