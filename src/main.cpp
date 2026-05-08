#include <Arduino.h>
#include <WiFi.h>
#include <iot_core/IIotCoreApp.h>

IIoTCoreAppPtr iotCoreApp;

void setup() {
  Serial.begin(115200);
  iotCoreApp = Implementation<IIoTCoreApp>::type::GetInstance();
}

void loop() {
  iotCoreApp->Loop();
}
