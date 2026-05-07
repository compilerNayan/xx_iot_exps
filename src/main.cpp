#include <Arduino.h>
#include <WiFi.h>
#include "IAwsIotCoreOperations.h"

#define WIFI_SSID "Garfield"
#define WIFI_PASSWORD "123Madhu$$"

/* @Autowired */
IAwsIotCoreOperationsPtr awsIotCoreOperations;

unsigned long lastReceiveMs = 0;
unsigned long lastSendMs = 0;

void connectToWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected. IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  connectToWiFi();
}

void loop() {
  unsigned long now = millis();

  if (now - lastReceiveMs >= 1000) {
    lastReceiveMs = now;

    if (awsIotCoreOperations != nullptr) {
      StdVector<StdString> messages = awsIotCoreOperations->ReceiveMessages();
      for (const auto& msg : messages) {
        Serial.print("Received: ");
        Serial.println(msg.c_str());
      }
    }
  }

  if (now - lastSendMs >= 5000) {
    lastSendMs = now;

    if (awsIotCoreOperations != nullptr) {
      Bool ok = awsIotCoreOperations->SendMessage("Hello");
      Serial.print("Send -> ");
      Serial.println(ok ? "OK" : "FAILED");
    }
  }
}
