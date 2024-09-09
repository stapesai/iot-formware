// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/*
 * @file include/BLEManager.hpp
 * @brief This file contains the declaration of the BLEManager class, which handles 
 *        Bluetooth Low Energy (BLE) communication for the device.
 * 
 * The BLEManager class is responsible for initializing the BLE device, advertising 
 * services, handling client connections and disconnections, and processing read 
 * and write requests for BLE characteristics.
 */
#ifndef BLE_MANGER_HPP
#define BLE_MANGER_HPP

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <NVSManager.hpp>
#include <Utils.hpp>
#include <functional>

class BLEManager : public BLECharacteristicCallbacks, public BLEServerCallbacks
{
private:
	NVSManager& config;
	Utils& utils;
	BLEServer* pServer;
	BLEService* pService;
	BLECharacteristic* pConfigCharacteristic;
	BLECharacteristic* pVersionCharacteristic;
	BLEAdvertising* pAdvertising;

public:
	BLEManager(NVSManager& config, Utils& utils);
	void initializeBLE();
	void restartBLEAdvertising();

	// Override BLECharacteristicCallbacks methods
	void onWrite(BLECharacteristic* pCharacteristic) override;
	void onRead(BLECharacteristic* pCharacteristic) override;

	// Override BLEServerCallbacks method
	void onConnect(BLEServer* pServer) override;
	void onDisconnect(BLEServer* pServer) override;

private:
	void handleConfigWrite(BLECharacteristic* pCharacteristic);
	void handleVersionRead(BLECharacteristic* pCharacteristic);
};

#endif // BLEMANGER_HPP