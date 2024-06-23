// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

// Path: include/BLEManger.hpp

#ifndef BLE_MANGER_HPP
#define BLE_MANGER_HPP

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <NVSManager.hpp>
#include <Utils.hpp>
#include <functional>

class BLEManager : public BLECharacteristicCallbacks
{
private:
	NVSManager& config;
	Utils& utils;
	BLEServer* pServer;
	BLEService* pService;
	BLECharacteristic* pCharacteristic;
	BLEAdvertising* pAdvertising;

public:
	BLEManager(NVSManager& config, Utils& utils);
	void initializeBLE();
	void onWrite(BLECharacteristic* pCharacteristic) override;
	void restartBLEAdvertising();
};

#endif // BLEMANGER_HPP
