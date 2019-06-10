#ifndef MqttWildcard_h
#define MqttWildcard_h

#include <Arduino.h>
#include <PubSubClientTools.h>

#ifndef TOPIC_BUFFER_SIZE
#define TOPIC_BUFFER_SIZE 100
#endif

class MqttWildcard {
    public:
        static int explode(String *results, String source, char delimiter);
        static bool wildcardMatch(String topic, String wildcard);
};

#endif