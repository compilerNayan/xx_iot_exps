#include <Arduino.h>
#include <WiFi.h>
#include "ArduinoFirebaseServer.h"

#define WIFI_SSID "Garfield"
#define WIFI_PASSWORD "123Madhu$$"

ArduinoFirebaseServer firebaseServer;

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
  Serial.println("[main] setup begin");
  connectToWiFi();
  Bool started = firebaseServer.Start();
  Serial.print("ArduinoFirebaseServer start -> ");
  Serial.println(started ? "OK" : "FAILED");
  Serial.println("[main] setup complete");
}

void loop() {
  unsigned long now = millis();

  if (now - lastReceiveMs >= 1000) {
    lastReceiveMs = now;
    Serial.println("[main] 1s poll -> ReceiveMessage()");

    IHttpRequestPtr request = firebaseServer.ReceiveMessage();
    if (request != nullptr) {
      Serial.print("Received: ");
      Serial.println(request->GetBody().c_str());
    } else {
      Serial.println("[main] ReceiveMessage() returned nullptr");
    }
  }

  if (now - lastSendMs >= 5000) {
    lastSendMs = now;

    StdString requestId = "kkm";
    StdString message = "Hello Nayan";
    Serial.print("[main] 5s publish trigger requestId=");
    Serial.print(requestId.c_str());
    Serial.print(" message=");
    Serial.println(message.c_str());
    Bool ok = firebaseServer.SendMessage(requestId, message);
    Serial.print("Send -> ");
    Serial.println(ok ? "OK" : "FAILED");
  }
}
