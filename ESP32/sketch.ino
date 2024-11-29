#include <UIPEthernet.h>
#include <SPI.h>
#include <ArduinoJson.h>
#include "Rtc.h"
#include "Mqtt.h"

#define BAUDRATE 115200

const char *broker = "192.168.0.100";
const int port = 1883;
const char *inputTopic = "/programacion_paralela/ethernet/in";
const char *outputTopic = "/programacion_paralela/ethernet/out";

// Ethernet Configuration
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress esp32IP(192, 168, 0, 120);
IPAddress arduinoIP(192, 168, 0, 130);
unsigned int udpPort = 8888;

void configEthernet();
void verifyEthernetStatus();
void handleReceivedUdpMessage();
void handleReceivedMessage(char *message);

EthernetUDP udp;
Rtc rtc;
Mqtt *mqtt = Mqtt::getInstance(broker, port, 128, inputTopic, outputTopic, handleReceivedMessage);

void setup() {
  Serial.begin(BAUDRATE);
  while (!Serial);

  rtc.init();
  configEthernet();
   mqtt->setup();
}

void loop() {
  mqtt->poll();
  verifyEthernetStatus();
  handleReceivedUdpMessage();
  delay(1000);
}

void configEthernet() {
  Serial.println(F("Initializing Ethernet..."));

  Ethernet.begin(mac, esp32IP);

  verifyEthernetStatus();

  Serial.print(F("IP Address: "));
  Serial.println(Ethernet.localIP());

  // Start UDP
  if (udp.begin(udpPort)) {
    Serial.println("UDP initialized successfully.");
  } else {
    Serial.println("Failed to initialize UDP.");
    while (true); // Stop execution
  }
}

void verifyEthernetStatus() {
  while (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println(F("Ethernet module not detected. Check connections."));
    delay(2000);
  }

  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println(F("Ethernet cable is not connected."));
    delay(2000);
  }
}

void handleReceivedMessage(char *message) {
  Serial.print(F("Message received (MQTT): "));
  Serial.println(message);

  StaticJsonDocument<128> newDoc;
  char buffer[128];

  newDoc["data"] = message;
  size_t len = serializeJson(newDoc, buffer, 128);

  udp.print(buffer);
}

void handleReceivedUdpMessage() {
  int packetSize = udp.parsePacket();
  if (packetSize > 0) {
    char buffer[128];
    int len = udp.read(buffer, sizeof(buffer) - 1);
    if (len > 0) {
      buffer[len] = '\0'; // Null-terminate the message
      Serial.print("Received message: ");
      Serial.println(buffer);

      StaticJsonDocument<128> jsonDoc;
      DeserializationError err = deserializeJson(jsonDoc, buffer);
      if (!err) {
        jsonDoc["esp32Timestamp"] = rtc.getTime();

        size_t len = serializeJson(jsonDoc, buffer, 128);
        mqtt->publishMessage(buffer);

      } else {
        Serial.print("JSON Parsing Error: ");
        Serial.println(err.c_str());
      }
    } else {
      Serial.println("Error reading UDP packet.");
    }
  } else {
    Serial.println("No UDP packets received.");
  }
}
