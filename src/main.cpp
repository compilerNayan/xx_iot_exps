#include <Arduino.h>
#include <iot_core/IIotCoreApp.h>

/* @Autowired */
IIoTCoreAppPtr iotCoreApp;
void setup() {
  Serial.begin(115200);
  iotCoreApp = Implementation<IIoTCoreApp>::type::GetInstance();
  iotCoreApp->Start();
}

void loop() {
  iotCoreApp->Loop();
}
