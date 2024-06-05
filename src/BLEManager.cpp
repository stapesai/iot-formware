// Path: src/BLEManager.cpp

#include <iostream>
#include <Arduino.h>
#include <BLEManager.h>

BLEConfigCallbacks::BLEConfigCallbacks(NVSManager &config, Utils &utils) : config(config), utils(utils), pServer(nullptr), pService(nullptr), pCharacteristic(nullptr), pAdvertising(nullptr) {}

void BLEConfigCallbacks::onWrite(BLECharacteristic* pCharacteristic)
{
    // Parse the characteristic value
    std::string value = pCharacteristic->getValue();
    std::cout << "Received value: " << value << std::endl;

    // Split the string into tokens
    std::vector<std::string> tokens = utils.formatString(value, ';');
    std::cout << "Number of tokens: " << tokens.size() << std::endl;

    // Setters for configuration parameters
    std::vector<std::function<void(const std::string &)>> configSetters = {
        [this](const std::string &v) { config.setWiFiSSID(v); },
        [this](const std::string &v) { config.setWiFiPassword(v); },
        [this](const std::string &v) { config.setMQTTBroker(v); },
        [this](const std::string &v) { config.setMQTTPort(v); },
        [this](const std::string &v) { config.setMQTTUser(v); },
        [this](const std::string &v) { config.setMQTTPassword(v); },
        [this](const std::string &v) { config.setMQTTTopic(v); }
    };

    // Check if the number of tokens matches the number of configuration parameters
    if (tokens.size() == configSetters.size())
    {
        // Assign tokens to configuration parameters
        for (size_t i = 0; i < configSetters.size(); i++)
        {
            std::cout << "Setting config parameter " << i << " to " << tokens[i] << std::endl;
            configSetters[i](tokens[i]);
        }
        // Debugging output
        utils.printConfig(config);

        // Write configuration to NVS
        config.writeConfig();
        std::cout << "Data written to NVS" << std::endl;

        // Restart the ESP32
        utils.restartDevice(1000);
    }
    else
    {
        std::cerr << "Mismatch between the number of tokens and the number of config parameters." << std::endl;
    }
}

void BLEConfigCallbacks::initialiseBLE()
{
    // Initialize BLE
    BLEDevice::init(utils.generateName());
    pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    pCharacteristic->setCallbacks(this);
    // pCharacteristic->addDescriptor(new BLE2902());
    pService->start();

    // Set up advertising
    pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x06); // Functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->start();
}

// void BLEConfigCallbacks::sendError(const std::string &error)
// {
//     pCharacteristic->setValue(error);
//     pCharacteristic->notify();
//     delay(500);
// }