#ifndef PubSubClientTools_h
#define PubSubClientTools_h

#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
#else
    #include <WProgram.h>
#endif

#include <inttypes.h>
#include "PubSubClient.h"

#define CALLBACK_SIGNATURE void (*callback)(String topic, String message)
#ifndef CLIENTID_BUFFER_SIZE
#define CLIENTID_BUFFER_SIZE 50
#endif
#ifndef TOPIC_BUFFER_SIZE
#define TOPIC_BUFFER_SIZE 100
#endif
#ifndef MESSAGE_BUFFER_SIZE
#define MESSAGE_BUFFER_SIZE 500
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
        PubSubClient* pubSub;
        struct callbackTopic callbackList[CALLBACK_LIST_SIZE];
        int callbackCount = 0;

        std::function<void(char*, unsigned char*, unsigned int)> mqtt_callback = std::bind(&PubSubClientTools::callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        void callback(char* topic_char, byte* payload, unsigned int length);

    public:
        PubSubClientTools(PubSubClient& pubSub);

        bool connect(String clientId);
        bool connect(String clientId, String willTopic, int willQoS, bool willRetain, String willMessage);

        bool publish(String topic, String message);
        bool publish(String topic, String message, bool retained);

        bool subscribe(String topic, CALLBACK_SIGNATURE);

        int resubscribe();

        // Static helper functions
        static int explode(String *results, String source, char delimiter);
        static bool wildcardMatch(String topic, String wildcard);
};

#endif
