#include "PubSubClientTools.h"

// Public
PubSubClientTools::PubSubClientTools(PubSubClient& _pubSub) {
    pubSub = &_pubSub;
    pubSub->setCallback(mqtt_callback);
};

bool PubSubClientTools::connect(String clientId) {
    char client_char[CLIENTID_BUFFER_SIZE];
    clientId.toCharArray(client_char, CLIENTID_BUFFER_SIZE);
    return pubSub->connect(client_char);
}
bool PubSubClientTools::connect(String clientId, String willTopic, int willQoS, bool willRetain, String willMessage) {
    char client_char[CLIENTID_BUFFER_SIZE];
    char topic_char[TOPIC_BUFFER_SIZE];
    char msg_char[MESSAGE_BUFFER_SIZE];

    clientId.toCharArray(client_char, CLIENTID_BUFFER_SIZE);
    willTopic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);
    willMessage.toCharArray(msg_char, MESSAGE_BUFFER_SIZE);

    return pubSub->connect(client_char, topic_char, willQoS, willRetain, msg_char);
}

bool PubSubClientTools::publish(String topic, String message) {
    return this->publish(topic, message, false);
}
bool PubSubClientTools::publish(String topic, String message, bool retained) {
    char topic_char[TOPIC_BUFFER_SIZE];
    char msg_char[MESSAGE_BUFFER_SIZE];

    topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);
    message.toCharArray(msg_char, MESSAGE_BUFFER_SIZE);

    return pubSub->publish(topic_char, msg_char, retained);
}

bool PubSubClientTools::subscribe(String topic, CALLBACK_SIGNATURE) {
    if (callbackCount >= CALLBACK_LIST_SIZE) return false;

    char topic_char[TOPIC_BUFFER_SIZE];
    topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);

    callbackList[callbackCount].topic = topic;
    callbackList[callbackCount].callback = callback;
    callbackCount++;

    return pubSub->subscribe(topic_char);
}

int PubSubClientTools::resubscribe() {
    int count = 0;

    pubSub->setCallback(mqtt_callback);

    for (int i = 0; i < callbackCount; i++) {
        char topic_char[TOPIC_BUFFER_SIZE];
        callbackList[i].topic.toCharArray(topic_char, TOPIC_BUFFER_SIZE);

        if ( pubSub->subscribe(topic_char) ) {
            count++;
        }
    }
    return count;
}

// Static helper functions
int PubSubClientTools::explode(String *results, String source, char delimiter) {
    int count = 0;
    int index = 0;
    
    for (int i = 0; i < source.length(); i++) {
        if (source.charAt(i) == delimiter) {
            results[count++] = source.substring(index, i);
            index = i+1;
        }
    }
    results[count++] = source.substring(index);

    return count;
}
bool PubSubClientTools::wildcardMatch(String topic, String wildcard) {
    // Catch trivial matches
    if (topic == wildcard) return true;
    if (wildcard == "#") return true;

    String exploded_topic[TOPIC_BUFFER_SIZE];
    int exploded_topic_count = PubSubClientTools::explode(exploded_topic, topic, '/');

    String exploded_wildcard[TOPIC_BUFFER_SIZE];
    int exploded_wildcard_count = PubSubClientTools::explode(exploded_wildcard, wildcard, '/');

    // Impossible to match since wildcard "+/+/#" is not matched by topic foo/bar
    if (exploded_wildcard_count > exploded_topic_count) return false;

    int match_count = 0;
    for (int i = 0; i < exploded_wildcard_count; i++) {
        if (exploded_wildcard[i] == "+") {
            continue;
        }
        if (exploded_wildcard[i] == "#") {
            return true;
        }
        if (exploded_wildcard[i] != exploded_topic[i]) {
            return false;
        }
    }

    /*
        If this point is reached and we did not return yet,
        topic- and wildcard-depth must be equal, otherwise it cant't be a valid match:
            topic:          foo/bar/example
            wildcard_1:     foo/bar/+
            wildcard_2:     foo/+
        Both wildcards would make it to this point, bot only wildcard_1 would be a valid match.
    */
    return (exploded_wildcard_count == exploded_topic_count);
}

// Private
void PubSubClientTools::callback(char* topic_char, byte* payload, unsigned int length) {
    String topic = String(topic_char);
    String message = "";
    for (int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    for (int i = 0; i < callbackCount; i++) {
        if (PubSubClientTools::wildcardMatch(topic, callbackList[i].topic)) {
            (*callbackList[i].callback)(topic,message);
        }
    }
}