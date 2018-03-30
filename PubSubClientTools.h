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
#define TOPIC_BUFFER_SIZE 100
#define MESSAGE_BUFFER_SIZE 500
#define CALLBACK_LIST_SIZE 50

struct callbackTopic {
	String topic;
	void (*callback)(String topic, String message);
};

class PubSubClientTools {
private:
	PubSubClient* pubSub;
	struct callbackTopic callbackList[CALLBACK_LIST_SIZE];
	int callbackCount = 0;
	std::function<void(char*, unsigned char*, unsigned int)> mqtt_callback = std::bind(&PubSubClientTools::_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	String prefix_publish;
	String prefix_subscribe;

public:
	PubSubClientTools(PubSubClient& pubSub);

	void setSubscribePrefix(String prefix);
	void setPublishPrefix(String prefix);

	void publish(String topic, String message);
	void subscribe(String topic, CALLBACK_SIGNATURE);
	int resubscribe();

	void _callback(char* topic_char, byte* payload, unsigned int length);
};

#endif
