// Path: include/BLEManger.h

#ifndef BLECONFIGCALLBACKS_H
#define BLECONFIGCALLBACKS_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <NVSManager.h>
#include <utils.h>
#include <functional>
class BLEConfigCallbacks : public BLECharacteristicCallbacks
{
private:
    BLEServer *pServer;
    BLEService *pService;
    BLECharacteristic *pCharacteristic;
    BLEAdvertising *pAdvertising;

public:
    BLEConfigCallbacks();
    void initialiseBLE(Utils &utils);
    void sendError(const std::string &error);
    void onWrite(NVSManager &config, Utils &utils);
};

#endif // BLECONFIGCALLBACKS_H
