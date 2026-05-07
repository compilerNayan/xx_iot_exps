#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

#define WIFI_SSID "Garfield"
#define WIFI_PASSWORD "123Madhu$$"

#define AWS_IOT_ENDPOINT "a2hlcpmplecdfa-ats.iot.us-east-1.amazonaws.com"
#define THING_NAME "nktestesp"

#define AWS_IOT_SUBSCRIBE_TOPIC "nknk32/sub"
#define AWS_IOT_PUBLISH_TOPIC "nknk32/pub"

static const char AWS_CERT_CA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

static const char AWS_CERT_CRT[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUQp8M9BjYXE+idNY/5RfHjjn6IAMwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDUwNzEzMzU0
MVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALhEYqYgbC1ScGYcnQ3m
i99U+1M+4P5EtoqXdErdksw1hlOb1eb9fw+QB8h5PqqGJZsms78GBMuv5gI+CHJU
pKBFmXYoDAB8dpYZX+K+nJyF6knOzauHjMlCu2h9zGBrqQBYvJLZS+jQr0TZgVi2
81lHZX+aesCPZuCCfMXZGWnrSD7fc6IPevwZaLDZEGPG8A/K5bl6c7KGjRNMJ5t/
GOKn4L5CzrTnJEUcuHuRa1Pdhuvht8NwNhYv6Wxs4svzQwhhcE3S1ROPmgFlFVg6
2fdCT16voxiDDl4C4aZAhxwyq0W7S9/7NUpzWZ56Jb/rmEO/MOTzcBg3SKhi1r4A
fTMCAwEAAaNgMF4wHwYDVR0jBBgwFoAUy1bwv08wSVTXzRwdV/DT54YC8JEwHQYD
VR0OBBYEFOFD0h3HUO3qjg48WutYEnmhrtMTMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQB+cBYXaJAcsTPePLgQjr6qDKa/
IQEVpjDhc03PPKxZNXROYd0OzHUuz5ZZzI0Oe0L61+kOr+nbH1HCoZviQluTyPbx
cnHmpkKs9Rvw6xEkEdGpq+krMuvy7XzE2pB5uRbeBt4eIMNwaWY1fhwHEfmepj4W
l8BtsbsYRx7ETouB0JyU1NOmab6TmHvnq5gpWsK94vkO8T9r0OccSGOup9fO8y7u
jA7ffKm53+CKcf5jT68VZWfO8QZFmUPn7NqxyScn5eu8FsVAYoQ1tW2RDG+boMvI
qe4M49X49JJDhluDRaLSjBn9JO27dTccEcQzTvxjSsGC/6Dn4aU80JDjy7A+
-----END CERTIFICATE-----
)KEY";

static const char AWS_CERT_PRIVATE[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEAuERipiBsLVJwZhydDeaL31T7Uz7g/kS2ipd0St2SzDWGU5vV
5v1/D5AHyHk+qoYlmyazvwYEy6/mAj4IclSkoEWZdigMAHx2lhlf4r6cnIXqSc7N
q4eMyUK7aH3MYGupAFi8ktlL6NCvRNmBWLbzWUdlf5p6wI9m4IJ8xdkZaetIPt9z
og96/BlosNkQY8bwD8rluXpzsoaNE0wnm38Y4qfgvkLOtOckRRy4e5FrU92G6+G3
w3A2Fi/pbGziy/NDCGFwTdLVE4+aAWUVWDrZ90JPXq+jGIMOXgLhpkCHHDKrRbtL
3/s1SnNZnnolv+uYQ78w5PNwGDdIqGLWvgB9MwIDAQABAoIBADU14jrQ0AxCaxzB
lITIF4oVc0H1vWkPlJru43EfDN4VcsXAEYPTnhZzZBv7DyKU/FUSVwOJjGb5U3VA
TpYdZTC3CHq7U8PsLR5pE2xZ2MXGUgKgSnoG+ZvAvneNJDCPxkzIUyQgxa/kW0iJ
erWQlzMnz+KKTHpArteUNs23RIR2JIPR1zsPbvaDJHzdZWSnKQOumtZleKYf1Nv8
9tJGC5NZi7OcKJlmS0Cz3H1dULDQWRvkR8Wqy8KybAXebVTPY33jRyy0lrdXy9mD
hPrUEuu3D2iTQw+qz4wH17iaO5g0tnhwo1JaohiMGmuMHAYIKBuWl53OaQ8GBr9M
uX0emIkCgYEA22p8s0qPhuyB29Ct9eW0XEK5Vkmo944PJts54xhqE0BWu2loZ9O6
+vmhzFqVANmBvIs9NVqB/y8+PB8mstT+7f42MOpHTLe6DiSGjYjx9YWYtjIo+xkB
ksNvC6+GH2MDXVQvr4RMSzI9+e8g1hv9mPVxTWG21v/25j0DY4EDIp8CgYEA1v2c
idDC2C6tARYydW8y5eYnJ/DqpQpP4wyjZvnPpMZZ+nLRSCN05rADCvQ6P1oVGVvg
NX7glj9zA17oSw5jGfMfNSmgExfoYw+XP+iX2nNJN1ncFa01cbe5GzPwVcDkHJ8p
+bxVuWvzs0KOkrEygpN8XXC7NpyZbDxVMxFgkO0CgYEAjusNzrsCQiBLdaK3rTOy
B2Zu0oWoSZd+xxCE5CdC8Mc3t1sxZm/DmzZ+7V0TMTp8BxOrCtWpV06uevrPeizC
4MJ1CeeQCaiiyBOCkS5ibEpY7lQ93VCsdaxW6IWsF0WnK2NqrCjAiTpwpBveh5gz
1y4IXx8GuYSAjfev/WBLaoECgYASRn6/DFrzy4vzUrK5XtmBx32+nHEl1GmUEGf3
gfbPt/jAHHCD5Fe0oHOxw68L4Zl3jWLSyVVIEF6zg6bVcoSkGRGnEEHGPbsIaLPf
v+Lgs/oeVWorAY1QfGCk2lZsw7fgYvtLemIDIBUYQOkjuSJlDSp+Ou3/J3K3S62m
aKy/xQKBgQCxLxXrvOineXzBeNXewSmtpkyUsrRbQRuTXkusSibZlAnFiVH5fnrM
gkssBwD5OAULIkPsefvrji0lc3/KQ8g+QUg/XBJZrnfj7bRi/XY6s7UqmNmxvZTK
ZgfIjvc/bSZo6CWAI1KvVUbJF7FDhSmcRX/Lrx7zVbeMjGbhZkRLng==
-----END RSA PRIVATE KEY-----
)KEY";

// Optional: public key from certs folder (not required by AWS IoT client auth).
static const char AWS_CERT_PUBLIC_KEY[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuERipiBsLVJwZhydDeaL
31T7Uz7g/kS2ipd0St2SzDWGU5vV5v1/D5AHyHk+qoYlmyazvwYEy6/mAj4IclSk
oEWZdigMAHx2lhlf4r6cnIXqSc7Nq4eMyUK7aH3MYGupAFi8ktlL6NCvRNmBWLbz
WUdlf5p6wI9m4IJ8xdkZaetIPt9zog96/BlosNkQY8bwD8rluXpzsoaNE0wnm38Y
4qfgvkLOtOckRRy4e5FrU92G6+G3w3A2Fi/pbGziy/NDCGFwTdLVE4+aAWUVWDrZ
90JPXq+jGIMOXgLhpkCHHDKrRbtL3/s1SnNZnnolv+uYQ78w5PNwGDdIqGLWvgB9
MwIDAQAB
-----END PUBLIC KEY-----
)KEY";

WiFiClientSecure secureClient;
PubSubClient mqttClient(secureClient);

ULong lastMqttPollMs = 0;
ULong lastPublishMs = 0;
UInt publishCounter = 0;

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
    if (mqttClient.connect(THING_NAME)) {
      Serial.println();
      Serial.println("Connected to AWS IoT");
      if (mqttClient.subscribe(AWS_IOT_SUBSCRIBE_TOPIC)) {
        Serial.print("Subscribed to: ");
        Serial.println(AWS_IOT_SUBSCRIBE_TOPIC);
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

  secureClient.setCACert(AWS_CERT_CA);
  secureClient.setCertificate(AWS_CERT_CRT);
  secureClient.setPrivateKey(AWS_CERT_PRIVATE);

  mqttClient.setServer(AWS_IOT_ENDPOINT, 8883);
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

  ULong now = millis();

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
    bool ok = mqttClient.publish(AWS_IOT_PUBLISH_TOPIC, message.c_str());
    Serial.print("Publish [");
    Serial.print(AWS_IOT_PUBLISH_TOPIC);
    Serial.print("]: ");
    Serial.print(message);
    Serial.print(" -> ");
    Serial.println(ok ? "OK" : "FAILED");
  }
}
