#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <PubSubClientTools.h>

#include <Thread.h>             // https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h>

#define WIFI_SSID "........"
#define WIFI_PASS "........"
#define MQTT_SERVER "broker.mqtt-dashboard.com"

WiFiClient espClient;
PubSubClient client(MQTT_SERVER, 1883, espClient);
PubSubClientTools mqtt(client);

ThreadController threadControl = ThreadController();
Thread thread = Thread();

int value = 0;
const String s = "";

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Connect to WiFi
  Serial.print(s+"Connecting to WiFi: "+WIFI_SSID+" ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");

  // Connect to MQTT
  Serial.print(s+"Connecting to MQTT: "+MQTT_SERVER+" ... ");
  if (client.connect("ESP8266Client")) {
    Serial.println("connected");

    mqtt.subscribe("test_in/foo/bar",  topic1_subscriber);
    mqtt.subscribe("test_in/+/bar",    topic2_subscriber);
    mqtt.subscribe("test_in/#",        topic3_subscriber);
  } else {
    Serial.println(s+"failed, rc="+client.state());
  }

  // Enable Thread
  thread.onRun(publisher);
  thread.setInterval(2000);
  threadControl.add(&thread);
}

void loop() {
  client.loop();
  threadControl.run();
}

void publisher() {
  ++value;
  mqtt.publish("test_out/hello_world", s+"Hello World! - No. "+value);
}
void topic1_subscriber(String topic, String message) {
  Serial.println(s+"Message arrived in function 1 ["+topic+"] "+message);
}
void topic2_subscriber(String topic, String message) {
  Serial.println(s+"Message arrived in function 2 ["+topic+"] "+message);
}
void topic3_subscriber(String topic, String message) {
  Serial.println(s+"Message arrived in function 3 ["+topic+"] "+message);
}
