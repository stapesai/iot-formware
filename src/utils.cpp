// Path: src/utils.h
// Description: This file contains the declaration of the Utils class.

#include <Arduino.h>
#include <nvs_flash.h>
#include <utils.h>

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

bool Utils::isNVSEmpty()
{
  nvs_handle_t nvsHandle;
  esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsHandle);
  if (err != ESP_OK)
  {
    Serial.println("Error opening NVS handle!");
    return false;
  }

  size_t required_size;
  err = nvs_get_blob(nvsHandle, "config", NULL, &required_size);
  if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
  {
    Serial.println("Error reading NVS blob!");
    return false;
  }

  nvs_close(nvsHandle);

  return required_size == 0;
}