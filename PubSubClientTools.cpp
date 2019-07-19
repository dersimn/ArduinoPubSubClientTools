#include "PubSubClientTools.h"

// Public
PubSubClientTools::PubSubClientTools(PubSubClient& _pubSub) : pubSub(_pubSub) {
    pubSub.setCallback(mqtt_callback);
};

bool PubSubClientTools::connected() {
    return pubSub.connected();
}
bool PubSubClientTools::connect(String clientId) {
    char client_char[CLIENTID_BUFFER_SIZE];
    clientId.toCharArray(client_char, CLIENTID_BUFFER_SIZE);
    return pubSub.connect(client_char);
}
bool PubSubClientTools::connect(String clientId, String willTopic, int willQoS, bool willRetain, String willMessage) {
    char client_char[CLIENTID_BUFFER_SIZE];
    char topic_char[TOPIC_BUFFER_SIZE];
    char msg_char[MESSAGE_BUFFER_SIZE];

    clientId.toCharArray(client_char, CLIENTID_BUFFER_SIZE);
    willTopic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);
    willMessage.toCharArray(msg_char, MESSAGE_BUFFER_SIZE);

    return pubSub.connect(client_char, topic_char, willQoS, willRetain, msg_char);
}

bool PubSubClientTools::publish(String topic, String message) {
    return this->publish(topic, message, false);
}
bool PubSubClientTools::publish(String topic, String message, bool retained) {
    char topic_char[TOPIC_BUFFER_SIZE];
    char msg_char[MESSAGE_BUFFER_SIZE];

    topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);
    message.toCharArray(msg_char, MESSAGE_BUFFER_SIZE);

    return pubSub.publish(topic_char, msg_char, retained);
}

bool PubSubClientTools::subscribe(String topic, CALLBACK_SIGNATURE) {
    if (callbackCount >= CALLBACK_LIST_SIZE) return false;

    char topic_char[TOPIC_BUFFER_SIZE];
    topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);

    callbackList[callbackCount].topic = topic;
    callbackList[callbackCount].callback = callback;
    callbackCount++;

    return pubSub.subscribe(topic_char);
}

int PubSubClientTools::resubscribe() {
    int count = 0;

    pubSub.setCallback(mqtt_callback);

    for (int i = 0; i < callbackCount; i++) {
        char topic_char[TOPIC_BUFFER_SIZE];
        callbackList[i].topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);

        if ( pubSub.subscribe(topic_char) ) {
            count++;
        }
    }
    return count;
}

// Private
void PubSubClientTools::callback(PUBSUBCLIENT_CALLBACK_PARAMETERS) {
    String topic = String(topicChar);
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    for (int i = 0; i < callbackCount; i++) {
        if (MqttWildcard::wildcardMatch(topic, callbackList[i].topic)) {
            (*callbackList[i].callback)(topic,message);
        }
    }
}