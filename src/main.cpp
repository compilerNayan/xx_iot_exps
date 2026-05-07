#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "IAwsIotCoreConfigProvider.h"

#define WIFI_SSID "Garfield"
#define WIFI_PASSWORD "123Madhu$$"

/* @Autowired */
IAwsIotCoreConfigProviderPtr awsConfigProvider;

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

unsigned long lastMqttPollMs = 0;
unsigned long lastPublishMs = 0;
unsigned int publishCounter = 0;

void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Incoming message on [");
  Serial.print(topic);
  Serial.print("]: ");

  for (unsigned int i = 0; i < length; ++i) {
    Serial.print(static_cast<char>(payload[i]));
  }
  Serial.println();
}

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

bool connectToAwsIot() {
  if (mqttClient.connected()) {
    return true;
  }

  Serial.print("Connecting to AWS IoT");
  while (!mqttClient.connected()) {
    if (mqttClient.connect(awsConfigProvider->GetThingName().c_str())) {
      Serial.println();
      Serial.println("Connected to AWS IoT");
      if (mqttClient.subscribe(awsConfigProvider->GetSubscribeTopic().c_str())) {
        Serial.print("Subscribed to: ");
        Serial.println(awsConfigProvider->GetSubscribeTopic().c_str());
      } else {
        Serial.println("Subscribe failed");
      }
      return true;
    }

    Serial.print(".");
    delay(1000);
  }

  return mqttClient.connected();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  connectToWiFi();

  secureClient.setCACert(awsConfigProvider->GetCaCert().c_str());
  secureClient.setCertificate(awsConfigProvider->GetDeviceCert().c_str());
  secureClient.setPrivateKey(awsConfigProvider->GetPrivateKey().c_str());

  mqttClient.setServer(awsConfigProvider->GetEndpoint().c_str(), 8883);
  mqttClient.setCallback(onMqttMessage);
  mqttClient.setBufferSize(1024);

  connectToAwsIot();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  if (!mqttClient.connected()) {
    connectToAwsIot();
  }

  unsigned long now = millis();

  // Check incoming messages every 1 second.
  if (now - lastMqttPollMs >= 1000) {
    lastMqttPollMs = now;
    mqttClient.loop();
  }

  // Publish "Hello N" every 5 seconds.
  if (now - lastPublishMs >= 5000) {
    lastPublishMs = now;
    publishCounter++;
    String message = "Hello " + String(publishCounter);
    bool ok = mqttClient.publish(awsConfigProvider->GetPublishTopic().c_str(), message.c_str());
    Serial.print("Publish [");
    Serial.print(awsConfigProvider->GetPublishTopic().c_str());
    Serial.print("]: ");
    Serial.print(message);
    Serial.print(" -> ");
    Serial.println(ok ? "OK" : "FAILED");
  }
}
