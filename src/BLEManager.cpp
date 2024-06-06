// Path: src/BLEManager.cpp
// TODO: Add BLE security, Make device only visible to `JarvisHome` app.

#include "BLEManager.hpp"
#include "Constants.hpp"
#include "Logger.hpp"
#include <Arduino.h>
#include <iostream>

BLEManager::BLEManager(NVSManager& config, Utils& utils)
	: config(config)
	, utils(utils)
	, pServer(nullptr)
	, pService(nullptr)
	, pCharacteristic(nullptr)
	, pAdvertising(nullptr)
{ }

void BLEManager::onWrite(BLECharacteristic* pCharacteristic)
{
	// Parse the characteristic value
	std::string value = pCharacteristic->getValue();
	Logger::log(INFO, GREEN "Received value: " RESET_COLOR + value);

	// Split the string into tokens
	std::vector<std::string> tokens = utils.formatString(value, ';');
	Logger::log(INFO, YELLOW "Number of tokens: " RESET_COLOR + String(tokens.size()));

	// Setters for configuration parameters
	std::vector<std::function<void(const std::string&)>> configSetters = {
		[this](const std::string& v) { config.setWiFiSSID(v); },
		[this](const std::string& v) { config.setWiFiPassword(v); },
		[this](const std::string& v) { config.setMQTTBroker(v); },
		[this](const std::string& v) { config.setMQTTPort(v); },
		[this](const std::string& v) { config.setMQTTUser(v); },
		[this](const std::string& v) { config.setMQTTPassword(v); },
		[this](const std::string& v) { config.setMQTTTopic(v); }};

	// Check if the number of tokens matches the number of configuration
	// parameters
	if(tokens.size() == configSetters.size())
	{
		// Assign tokens to configuration parameters
		for(size_t i = 0; i < configSetters.size(); i++)
		{
			Logger::log(INFO,
						YELLOW "Setting config parameter " + String(i) + " to " +
							String(tokens[i].c_str()) + RESET_COLOR);
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
		Logger::log(
			ERROR,
			RED "Number of tokens does not match number of configuration parameters." RESET_COLOR);

		// Restart advertising on error
		restartBLEAdvertising();
	}
}

void BLEManager::initializeBLE()
{
	// Initialize BLE
	BLEDevice::init(utils.generateName());
	pServer = BLEDevice::createServer();
	pService = pServer->createService(SERVICE_UUID);
	pCharacteristic =
		pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE);
	pCharacteristic->setCallbacks(this);
	// pCharacteristic->addDescriptor(new BLE2902());

	// Implement security: require bonding, MITM protection, etc.
	BLESecurity* pSecurity = new BLESecurity();
	pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
	pSecurity->setCapability(ESP_IO_CAP_IO);

	// Start the service
	pService->start();

	// Set up advertising
	pAdvertising = BLEDevice::getAdvertising();
	pAdvertising->addServiceUUID(SERVICE_UUID);
	pAdvertising->setScanResponse(false);
	pAdvertising->setMinPreferred(0x06); // Functions that help with iPhone connections issue
	pAdvertising->setMinPreferred(0x12);
	pAdvertising->start();
}

void BLEManager::restartBLEAdvertising()
{

	if(pAdvertising != nullptr)
	{
		pAdvertising->stop();
		pAdvertising->start();
		Logger::log(INFO, GREEN "Restarted advertising." RESET_COLOR);
	}
	else
	{
		Logger::log(ERROR, RED "Advertising object is null." RESET_COLOR);
	}
}