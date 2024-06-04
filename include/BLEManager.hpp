// Path: include/BLEManger.hpp

#ifndef BLE_MANGER_HPP
#define BLE_MANGER_HPP

#include "NVSManager.hpp"
#include "Utils.hpp"
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <functional>

class BLEManager : public BLECharacteristicCallbacks {
  private:
    NVSManager &config;
    Utils &utils;
    BLEServer *pServer;
    BLEService *pService;
    BLECharacteristic *pCharacteristic;
    BLEAdvertising *pAdvertising;

  public:
    BLEManager(NVSManager &config, Utils &utils);
    void initializeBLE();
    void sendError(const std::string &error);
    void onWrite(BLECharacteristic *pCharacteristic) override;
    void restartBLEAdvertising();
};

#endif // BLEMANGER_HPP
