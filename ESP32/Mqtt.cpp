#include "Mqtt.h"

EthernetClient Mqtt::ethernetClient;
MqttClient Mqtt::mqttClient(Mqtt::ethernetClient);
Mqtt *Mqtt::instance = nullptr;
char *Mqtt::buffer = nullptr;
size_t Mqtt::bufferSize = 0;
std::function<void(char *)> Mqtt::callback = nullptr;

Mqtt::Mqtt(const char *broker, int port, size_t bufferSize, const char *inputTopic, const char *outputTopic, std::function<void(char *)> callback)
    : broker(broker), port(port), inputTopic(inputTopic), outputTopic(outputTopic) {
  Mqtt::bufferSize = bufferSize;
  Mqtt::buffer = new char[bufferSize];
  Mqtt::callback = callback;
}

Mqtt::~Mqtt() {
  mqttClient.stop();
  delete[] Mqtt::buffer;
}

Mqtt *Mqtt::getInstance(
    const char *broker, int port, size_t bufferSize,
    const char *inputTopic, const char *outputTopic, std::function<void(char *)> callback) {
  if (!instance) {
    instance = new Mqtt(broker, port, bufferSize, inputTopic, outputTopic, callback);
  }

  return instance;
}

void Mqtt::destroyInstance() {
  if (instance) {
    delete instance;
    instance = nullptr;
  }
}

void Mqtt::setup() {
  initMqttClient();
}

void Mqtt::publishMessage(const char *message) {
  mqttClient.beginMessage(outputTopic);
  mqttClient.print(message);
  mqttClient.endMessage();
}

void Mqtt::poll() {
  mqttClient.poll();
}

void Mqtt::initMqttClient() {
  if (!mqttClient.connect(broker, port)) {
    Serial.print(F("MQTT connection failed! Error code = "));
    Serial.println(mqttClient.connectError());
    while (true)
      ;
  }

  Serial.println(F("Connected to the MQTT broker"));

  while (!mqttClient.subscribe(inputTopic)) {
    Serial.print(F("Failed to subscribe to topic: "));
    Serial.println(inputTopic);
    delay(2000);
  }

  mqttClient.onMessage(onMessage);
}

void Mqtt::onMessage(int messageSize) {
  if (messageSize > 0) {
    if (!buffer || bufferSize == 0) {
      Serial.println(F("Buffer is not initialized."));
      return;
    }

    int index = 0;
    while (mqttClient.available() && index < bufferSize - 1) {
      buffer[index++] = (char)mqttClient.read();
    }

    buffer[index] = '\0';

    if (callback) {
      callback(buffer);
    }
  }
}
