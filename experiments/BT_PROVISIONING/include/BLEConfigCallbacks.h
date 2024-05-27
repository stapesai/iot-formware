#ifndef BLECONFIGCALLBACKS_H
#define BLECONFIGCALLBACKS_H

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "ManageConfig.h"

class BLEConfigCallbacks : public BLECharacteristicCallbacks
{
public:
    BLEConfigCallbacks(ManageConfig &config);
    void onWrite(BLECharacteristic *pCharacteristic) override;

private:
    ManageConfig &config;
};

#endif // BLECONFIGCALLBACKS_H
