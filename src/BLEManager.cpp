// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: src/BLEManager.cpp
// TODO: Add BLE security, Make device only visible to `JarvisHome` app.

#include <Arduino.h>
#include <BLEManager.hpp>
#include <Constants.hpp>
#include <Logger.hpp>
#include <iostream>

BLEManager::BLEManager(NVSManager& config, Utils& utils)
	: config(config)
	, utils(utils)
	, pServer(nullptr)
	, pService(nullptr)
	, pConfigCharacteristic(nullptr)
	, pVersionCharacteristic(nullptr)
	, pAdvertising(nullptr)
{ }

void BLEManager::onWrite(BLECharacteristic* pCharacteristic)
{
	if(pCharacteristic->getUUID().toString() == CONFIG_CHARACTERISTIC_UUID)
	{
		handleConfigWrite(pCharacteristic);
	}
}

void BLEManager::onRead(BLECharacteristic* pCharacteristic)
{
	if(pCharacteristic->getUUID().toString() == VERSION_CHARACTERISTIC_UUID)
	{
		handleVersionRead(pCharacteristic);
	}
}

void BLEManager::onConnect(BLEServer* pServer)
{
	Logger::log(INFO, GREEN "Client connected" RESET_COLOR);
}

void BLEManager::onDisconnect(BLEServer* pServer)
{
	Logger::log(INFO, YELLOW "Client disconnected" RESET_COLOR);
	pAdvertising->start();
}

void BLEManager::handleConfigWrite(BLECharacteristic* pCharacteristic)
{
	// Parse the characteristic value
	std::string value = pCharacteristic->getValue();
	Logger::log(INFO, GREEN "Received configuration: " RESET_COLOR + value);

	// Parse the key-value pairs
	std::map<std::string, std::string> kv_map = utils.convertToKVMap(value);
	Logger::log(INFO, YELLOW "Number of key-value pairs: " RESET_COLOR + String(kv_map.size()));

	// Setters for configuration parameters
	std::map<std::string, std::function<void(const std::string&)>> configSetters = {{"WIFI_SSID", [this](const std::string& v) { config.setWiFiSSID(v); }},
																					{"WIFI_PASSWORD", [this](const std::string& v) { config.setWiFiPassword(v); }},
																					{"MQTT_BROKER", [this](const std::string& v) { config.setMQTTBroker(v); }},
																					{"MQTT_PORT", [this](const std::string& v) { config.setMQTTPort(v); }},
																					{"MQTT_USERNAME", [this](const std::string& v) { config.setMQTTUsername(v); }},
																					{"MQTT_PASSWORD", [this](const std::string& v) { config.setMQTTPassword(v); }},
																					{"USER_ID", [this](const std::string& v) { config.setUserId(v); }}};

	// Check if all required keys are present
	bool allKeysPresent = true;
	for(const auto& setter : configSetters)
	{
		if(kv_map.find(setter.first) == kv_map.end())
		{
			allKeysPresent = false;
			break;
		}
	}

	if(allKeysPresent)
	{
		// Assign values to configuration parameters
		for(const auto& setter : configSetters)
		{
			const std::string& key = setter.first;
			const std::string& value = kv_map[key];
			Logger::log(INFO, YELLOW "Setting config parameter " + String(key.c_str()) + " to " + String(value.c_str()) + RESET_COLOR);
			setter.second(value);
		}
		// Debugging output
		utils.printConfig(config);

		// Write configuration to NVS
		config.writeConfig();
		Logger::log(INFO, GREEN "Configuration written to NVS." RESET_COLOR);

		// Stop advertising
		pAdvertising->stop();

		// Restart the ESP32
		utils.restartDevice(1000);
	}
	else
	{
		Logger::log(ERROR, RED "Missing one or more configuration parameters." RESET_COLOR);

		// Restart advertising on error
		restartBLEAdvertising();
	}
}

void BLEManager::handleVersionRead(BLECharacteristic* pCharacteristic)
{
	std::string versionInfo = "HARDWARE_CHIP=" + std::string(HARDWARE_CHIP) + ";HARDWARE_MAC_ADDRESS=" + utils.getMacAddr() +
							  ";HARDWARE_VERSION=" + std::string(HARDWARE_VERSION) + ";FIRMWARE_VERSION=" + std::string(FIRMWARE_VERSION);
	pCharacteristic->setValue(versionInfo);
	Logger::log(INFO, GREEN "Sent version info: " + String(versionInfo.c_str()) + RESET_COLOR);
}

void BLEManager::initializeBLE()
{
	// Initialize BLE
	std::string deviceName = BLE_DEVICE_NAME + utils.getMacAddr();
	BLEDevice::init(deviceName.c_str());
	pServer = BLEDevice::createServer();
	pServer->setCallbacks(this);
	pService = pServer->createService(SERVICE_UUID);

	// Create configuration characteristic
	pConfigCharacteristic = pService->createCharacteristic(CONFIG_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
	pConfigCharacteristic->setCallbacks(this);

	// Create version characteristic
	pVersionCharacteristic = pService->createCharacteristic(VERSION_CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ);
	pVersionCharacteristic->setCallbacks(this);

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
