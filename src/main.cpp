#include <Arduino.h>
#include <iot_core/IIotCoreApp.h>
#include "device/IDeviceCollection.h"

IIoTCoreAppPtr iotCoreApp;

/* @Autowired */
IDeviceCollectionPtr deviceCollection;

void setup() {
  Serial.begin(115200);
  Serial.println("dsds jdeoang-un");
  iotCoreApp = Implementation<IIoTCoreApp>::type::GetInstance();
  iotCoreApp->Start();
}

void loop() {
  //Serial.println("ddd joang-un");
  iotCoreApp->Loop();
  deviceCollection->RefreshAllDevices();
  delay(1000);
}

