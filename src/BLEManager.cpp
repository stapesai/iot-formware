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
	, pCharacteristic(nullptr)
	, pAdvertising(nullptr)
{ }

void BLEManager::onWrite(BLECharacteristic* pCharacteristic)
{
	// Parse the characteristic value
	std::string value = pCharacteristic->getValue();
	Logger::log(INFO, GREEN "Received value: " RESET_COLOR + value);

	// Parse the key-value pairs
	std::map<std::string, std::string> kv_map = utils.convertToKVMap(value);
	Logger::log(INFO, YELLOW "Number of key-value pairs: " RESET_COLOR + String(kv_map.size()));

	// Setters for configuration parameters
	std::map<std::string, std::function<void(const std::string&)>> configSetters = {
		{"WIFI_SSID", [this](const std::string& v) { config.setWiFiSSID(v); }},
		{"WIFI_PASSWORD", [this](const std::string& v) { config.setWiFiPassword(v); }},
		{"MQTT_BROKER", [this](const std::string& v) { config.setMQTTBroker(v); }},
		{"MQTT_PORT", [this](const std::string& v) { config.setMQTTPort(v); }},
		{"MQTT_PASSWORD", [this](const std::string& v) { config.setMQTTPassword(v); }},
		{"USER_ID", [this](const std::string& v) { config.setUserId(v); }},
		{"HOME_ID", [this](const std::string& v) { config.setHomeId(v); }}};

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
			Logger::log(INFO,
						YELLOW "Setting config parameter " + String(key.c_str()) + " to " +
							String(value.c_str()) + RESET_COLOR);
			setter.second(value);
		}
		// Debugging output
		utils.printConfig(config);

		// Write configuration to NVS
		config.writeConfig();
		Logger::log(INFO, GREEN "Configuration written to NVS." RESET_COLOR);

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

void BLEManager::initializeBLE()
{
	// Initialize BLE
	std::string deviceName = BLE_DEVICE_NAME + utils.getMacAddr();
	BLEDevice::init(deviceName.c_str());
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
