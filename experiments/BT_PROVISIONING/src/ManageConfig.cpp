#include "ManageConfig.h"
#include <nvs.h>
#include <nvs_flash.h>
#include <Arduino.h>
#include <iostream>

ManageConfig::ManageConfig()
{
    fields = {&ssid, &password, &mqtt_server, &mqtt_port, &mqtt_user, &mqtt_password, &mqtt_topic};
}

void ManageConfig::writeConfig()
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        Serial.println("Error opening NVS handle");
        return;
    }

    for (int i = 0; i < 7; ++i)
    {
        err = nvs_set_str(my_handle, keys[i], fields[i]->c_str());
        if (err != ESP_OK)
        {
            Serial.printf("Error writing to NVS for field %d (%s)\n", i, keys[i]);
        }
    }

    err = nvs_commit(my_handle);
    if (err != ESP_OK)
    {
        Serial.println("Error committing NVS changes");
    }
    nvs_close(my_handle);
}

void ManageConfig::readConfig()
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        Serial.println("Error opening NVS handle");
        return;
    }

    for (int i = 0; i < 7; ++i)
    {
        size_t required_size;
        err = nvs_get_str(my_handle, keys[i], NULL, &required_size);
        if (err != ESP_OK)
        {
            Serial.printf("Error reading size from NVS for field %d (%s)\n", i, keys[i]);
            continue;
        }

        char *value = (char *)malloc(required_size);
        if (value == nullptr)
        {
            Serial.println("Failed to allocate memory for NVS read");
            continue;
        }

        err = nvs_get_str(my_handle, keys[i], value, &required_size);
        if (err == ESP_OK)
        {
            *fields[i] = std::string(value);
            std::cout << "Read from NVS: " << keys[i] << " = " << *(fields[i]) << std::endl;
        }
        else
        {
            Serial.printf("Error reading value from NVS for field %d (%s)\n", i, keys[i]);
        }
        free(value);
    }
    nvs_close(my_handle);
}
