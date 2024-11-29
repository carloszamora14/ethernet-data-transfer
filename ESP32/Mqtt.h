#ifndef MQTT_H
#define MQTT_H

#include <ArduinoMqttClient.h>
#include <UIPEthernet.h>
#include <functional>

class Mqtt {
private:
  static EthernetClient ethernetClient;
  static MqttClient mqttClient;
  static Mqtt *instance;

  const char *broker;
  const char *inputTopic;
  const char *outputTopic;
  static char *buffer;
  static size_t bufferSize;
  static std::function<void(char *)> callback;
  int port;

  Mqtt(const char *broker, int port, size_t bufferSize, const char *inputTopic, const char *outputTopic, std::function<void(char*)> callback);
  ~Mqtt();

public:
  static Mqtt *getInstance(const char *broker, int port, size_t bufferSize, const char *inputTopic, const char *outputTopic, std::function<void(char *)> callback);
  static void destroyInstance();
  void setup();
  void publishMessage(const char *message);
  void poll();

private:
  void initMqttClient();
  static void onMessage(int messageSize);
};

#endif
