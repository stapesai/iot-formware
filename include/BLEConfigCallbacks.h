// Path: include/BLEConfigCallbacks.h

#ifndef BLECONFIGCALLBACKS_H
#define BLECONFIGCALLBACKS_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "ManageConfig.h"

class BLEConfigCallbacks : public BLECharacteristicCallbacks
{
private:
    ManageConfig &config;

public:
    BLEConfigCallbacks(ManageConfig &config);
    void onWrite(BLECharacteristic *pCharacteristic) override;
};

#endif // BLECONFIGCALLBACKS_H
