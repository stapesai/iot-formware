// Path: include/BLEManger.hpp

#ifndef BLECONFIGCALLBACKS_H
#define BLECONFIGCALLBACKS_H

#include "NVSManager.hpp"
#include "Utils.hpp"
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <functional>

class BLEConfigCallbacks : public BLECharacteristicCallbacks {
  private:
    NVSManager &config;
    Utils &utils;
    BLEServer *pServer;
    BLEService *pService;
    BLECharacteristic *pCharacteristic;
    BLEAdvertising *pAdvertising;

  public:
    BLEConfigCallbacks(NVSManager &config, Utils &utils);
    void initializeBLE();
    void sendError(const std::string &error);
    void onWrite(BLECharacteristic *pCharacteristic) override;
    void restartBLEAdvertising();
};

#endif // BLECONFIGCALLBACKS_H
