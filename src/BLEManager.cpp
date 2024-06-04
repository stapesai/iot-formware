#include <iostream>
#include <Arduino.h>
#include <BLEManager.h>

BLEConfigCallbacks::BLEConfigCallbacks(NVSManager &config, Utils &utils) : config(config), utils(utils) {}

void BLEConfigCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    // Parse the characteristic value
    std::string value = pCharacteristic->getValue();
    std::vector<std::string> tokens = utils.formatString(value, ';');
    std::vector<std::function<void(const std::string &)>> configSetters = {
        [this](const std::string &v)
        { config.setWiFiSSID(v); },
        [this](const std::string &v)
        { config.setWiFiPassword(v); },
        [this](const std::string &v)
        { config.setMQTTBroker(v); },
        [this](const std::string &v)
        { config.setMQTTPort(v); },
        [this](const std::string &v)
        { config.setMQTTUser(v); },
        [this](const std::string &v)
        { config.setMQTTPassword(v); },
        [this](const std::string &v)
        { config.setMQTTTopic(v); }};

    if (tokens.size() == configSetters.size())
    {
        // Assign tokens to configuration parameters

        for (size_t i = 0; i < configSetters.size(); i++)
        {
            configSetters[i](tokens[i]);
        }
        // Debugging output
        std::cout << "SSID: " << config.getWiFiSSID() << std::endl;
        std::cout << "Password: " << config.getWiFiPassword() << std::endl;
        std::cout << "MQTT Server: " << config.getMQTTBroker() << std::endl;
        std::cout << "MQTT Port: " << config.getMQTTPort() << std::endl;
        std::cout << "MQTT User: " << config.getMQTTUser() << std::endl;
        std::cout << "MQTT Password: " << config.getMQTTPassword() << std::endl;
        std::cout << "MQTT Topic: " << config.getMQTTTopic() << std::endl;

        // Write configuration to NVS
        config.writeConfig();
        std::cout << "Data written to NVS" << std::endl;

        // Restart the ESP32
        std::cout << "Restarting ESP32..." << std::endl;
        delay(1000);
        esp_restart();
    }
    else
    {
        std::cerr << "Mismatch between the number of tokens and the number of config parameters." << std::endl;
        return;
    }
}

void BLEConfigCallbacks::initialiseBLE()
{
    // Initialize BLE
    BLEDevice::init(utils.generateName());
    pServer = BLEDevice::createServer();
    pServer->createService(SERVICE_UUID);
    pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
    pCharacteristic->setCallbacks(new BLEConfigCallbacks(config, utils));
    pService->start();

    // Set up advertising
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->start();
}

void BLEConfigCallbacks::deinitaliseBLE()
{
    pService->stop();
    pAdvertising->stop();
    BLEDevice::deinit(true);
}

void BLEConfigCallbacks::sendError(std::string error)
{
    pCharacteristic->setValue(error.c_str());
    pCharacteristic->notify();
}