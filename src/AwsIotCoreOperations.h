#ifndef AWSIOTCOREOPERATIONS_H
#define AWSIOTCOREOPERATIONS_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "IAwsIotCoreOperations.h"
#include "IAwsIotCoreConfigProvider.h"

/* @Component */
class AwsIotCoreOperations : public IAwsIotCoreOperations {
    /* @Autowired */
    Private IAwsIotCoreConfigProviderPtr configProvider;

    Private WiFiClientSecure secureClient;
    Private PubSubClient mqttClient;

    Private StdString endpoint;
    Private StdString thingName;
    Private StdString caCert;
    Private StdString deviceCert;
    Private StdString privateKey;
    Private StdString publishTopic;
    Private StdString subscribeTopic;

    Private Bool configured = false;
    Private StdMap<StdString, StdVector<StdString>> bufferedMessages;
    Private StdUnorderedSet<StdString> subscribedTopics;

    Private Static AwsIotCoreOperations* activeInstance;

    Private Static Void StaticMqttCallback(Char* topic, UInt8* payload, UInt length) {
        if (activeInstance != nullptr) {
            activeInstance->OnMqttMessage(topic, payload, length);
        }
    }

    Private Void OnMqttMessage(Char* topic, UInt8* payload, UInt length) {
        if (topic == nullptr) {
            return;
        }

        StdString topicName(topic);
        StdString message;
        message.reserve(length);
        for (UInt i = 0; i < length; ++i) {
            message += static_cast<Char>(payload[i]);
        }
        bufferedMessages[topicName].push_back(message);
    }

    Private Bool EnsureConfigured() {
        if (configured) {
            return true;
        }
        if (configProvider == nullptr) {
            return false;
        }

        endpoint = configProvider->GetEndpoint();
        thingName = configProvider->GetThingName();
        caCert = configProvider->GetCaCert();
        deviceCert = configProvider->GetDeviceCert();
        privateKey = configProvider->GetPrivateKey();
        publishTopic = configProvider->GetPublishTopic();
        subscribeTopic = configProvider->GetSubscribeTopic();

        secureClient.setCACert(caCert.c_str());
        secureClient.setCertificate(deviceCert.c_str());
        secureClient.setPrivateKey(privateKey.c_str());

        mqttClient.setServer(endpoint.c_str(), 8883);
        mqttClient.setCallback(StaticMqttCallback);
        mqttClient.setBufferSize(1024);

        activeInstance = this;
        configured = true;
        return true;
    }

    Private Bool EnsureMqttConnected() {
        if (!EnsureConfigured()) {
            return false;
        }
        if (WiFi.status() != WL_CONNECTED) {
            return false;
        }
        if (mqttClient.connected()) {
            return true;
        }
        return mqttClient.connect(thingName.c_str());
    }

    Private Bool EnsureSubscribed(CStdString topicName) {
        if (!EnsureMqttConnected()) {
            return false;
        }
        if (subscribedTopics.find(topicName) != subscribedTopics.end()) {
            return true;
        }
        if (mqttClient.subscribe(topicName.c_str())) {
            subscribedTopics.insert(topicName);
            return true;
        }
        return false;
    }

    Public AwsIotCoreOperations() : mqttClient(secureClient) {}
    Public Virtual ~AwsIotCoreOperations() override = default;

    Public Virtual Bool SendMessage(CStdString message) override {
        return SendMessage(message, publishTopic);
    }

    Public Virtual StdVector<StdString> ReceiveMessages() override {
        return ReceiveMessages(subscribeTopic);
    }

    Public Virtual Bool SendMessage(CStdString message, CStdString topicName) override {
        if (!EnsureMqttConnected()) {
            return false;
        }
        mqttClient.loop();
        return mqttClient.publish(topicName.c_str(), message.c_str());
    }

    Public Virtual StdVector<StdString> ReceiveMessages(CStdString topicName) override {
        StdVector<StdString> result;
        if (!EnsureSubscribed(topicName)) {
            return result;
        }

        mqttClient.loop();

        auto it = bufferedMessages.find(topicName);
        if (it == bufferedMessages.end()) {
            return result;
        }

        result = it->second;
        bufferedMessages.erase(it);
        return result;
    }
};

AwsIotCoreOperations* AwsIotCoreOperations::activeInstance = nullptr;

#endif /* AWSIOTCOREOPERATIONS_H */
