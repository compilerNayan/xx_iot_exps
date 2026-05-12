#include <Arduino.h>
#include <iot_core/IIotCoreApp.h>
#include "device/IDeviceCollection.h"
#include "auth/IEndpointSecurityConfig.h"
#include "HttpMethod.h"
#include "TestAuthFilter.h"
IIoTCoreAppPtr iotCoreApp;

/* @Autowired */
IDeviceCollectionPtr deviceCollection;

/* @Autowired */
IEndpointSecurityConfigPtr endpointSecurityConfig;

void setup() {
  Serial.begin(115200);
  Serial.println("dsds jdeoang-un");
  iotCoreApp = Implementation<IIoTCoreApp>::type::GetInstance();
  endpointSecurityConfig->AddRule<TestAuthFilter>("/switch/{id}", HttpMethod::GET);
  iotCoreApp->Start();
}

void loop() {
  //Serial.println("ddd joang-un");
  iotCoreApp->Loop();
  deviceCollection->RefreshAllDevices();
  delay(1000);
}

