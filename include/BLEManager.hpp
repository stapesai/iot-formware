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