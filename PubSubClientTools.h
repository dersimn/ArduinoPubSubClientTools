#ifndef PubSubClientTools_h
#define PubSubClientTools_h

#if !defined(ESP8266) && !defined(ESP32)
#warning This library was developed for ESP8266 and ESP32 microcontrollers
#endif

#include <Arduino.h>

#include <inttypes.h>
#include "PubSubClient.h"
#include "MqttWildcard.h"

#define CALLBACK_SIGNATURE void (*callback)(String topic, String message)
#define PUBSUBCLIENT_CALLBACK_PARAMETERS char* topicChar, uint8_t* payload, unsigned int length

#ifndef CLIENTID_BUFFER_SIZE
#define CLIENTID_BUFFER_SIZE 50
#endif
#ifndef TOPIC_BUFFER_SIZE
#define TOPIC_BUFFER_SIZE 100
#endif
#ifndef MESSAGE_BUFFER_SIZE
#define MESSAGE_BUFFER_SIZE MQTT_MAX_PACKET_SIZE
#endif
#ifndef CALLBACK_LIST_SIZE
#define CALLBACK_LIST_SIZE 50
#endif

struct callbackTopic {
    String topic;
    void (*callback)(String topic, String message);
};

class PubSubClientTools {
    private:
        PubSubClient& pubSub;
        struct callbackTopic callbackList[CALLBACK_LIST_SIZE];
        int callbackCount = 0;

        std::function<void(PUBSUBCLIENT_CALLBACK_PARAMETERS)> mqtt_callback = std::bind(&PubSubClientTools::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        void callback(PUBSUBCLIENT_CALLBACK_PARAMETERS);

    public:
        PubSubClientTools(PubSubClient& pubSub);

        bool connected();
        bool connect(String clientId);
        bool connect(String clientId, String willTopic, int willQoS, bool willRetain, String willMessage);

        bool publish(String topic, String message);
        bool publish(String topic, String message, bool retained);

        bool subscribe(String topic, CALLBACK_SIGNATURE);

        int resubscribe();
};

#endif
