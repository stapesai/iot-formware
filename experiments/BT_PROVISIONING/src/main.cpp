#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <nvs_flash.h>
#include "ManageConfig.h"
#include "BLEConfigCallbacks.h"

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

ManageConfig config;

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

    pCharacteristic->setCallbacks(new BLEConfigCallbacks(config));
    pService->start();
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
    config.readConfig();

    if (!config.getSSID().empty())
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
    static unsigned long buttonPressStartTime = 0;
    bool buttonPressed = digitalRead(0) == LOW;

    if (buttonPressed)
    {
        if (buttonPressStartTime == 0)
        {
            buttonPressStartTime = millis();
        }
        else if (millis() - buttonPressStartTime >= 5000)
        {
            Serial.println("Resetting NVS");

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
    }
    else
    {
        buttonPressStartTime = 0;
    }
}
