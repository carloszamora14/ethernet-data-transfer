#include "Rtc.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <UIPEthernet.h>

// Network Configuration
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE};
IPAddress arduinoIP(192, 168, 0, 130);
IPAddress remoteIP(192, 168, 0, 120);
unsigned int udpPort = 8888;

EthernetUDP udp;
Rtc rtc; // Real-time clock instance

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ; // Wait for serial initialization

  // Initialize Ethernet and UDP
  Ethernet.begin(mac, arduinoIP);
  udp.begin(udpPort);

  // Verify Ethernet connectivity
  verifyEthernetStatus();

  Serial.print("IP Address: ");
  Serial.println(Ethernet.localIP());

  // Initialize RTC
  rtc.init();
}

void loop() {
  verifyEthernetStatus(); // Check Ethernet connection

  sendTimestampMessage(); // Send the timestamp message

  delay(1000); // Delay for 1 second before next loop
}

void sendTimestampMessage() {
  StaticJsonDocument<64> newDoc;
  Serial.println(rtc.getTime());
  newDoc["arduinoTimestamp"] = rtc.getTime(); // Add RTC timestamp
  char buffer[64];

  // Serialize JSON to buffer
  size_t len = serializeJson(newDoc, buffer, 64);

  // Send the UDP packet
  udp.beginPacket(remoteIP, udpPort);
  if (udp.print(buffer) > 0 && udp.endPacket()) {
    Serial.print("Sent: ");
    Serial.println(buffer);
  } else {
    Serial.println("Failed to send message.");
  }
}

void handleReceivedMessage() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    StaticJsonDocument<128> newDoc;
    char buffer[128];

    int len = udp.read(buffer, sizeof(buffer) - 1);
    if (len > 0) {
      buffer[len] = '\0'; // Null-terminate the message
      Serial.print("Received: ");
      Serial.println(buffer);

      // Deserialize the received JSON
      DeserializationError err = deserializeJson(newDoc, buffer);
      if (!err) {
        const char *data = newDoc["data"];
        if (data) {
          Serial.print("Data field: ");
          Serial.println(data);
        }
      } else {
        Serial.print("JSON Parsing Error: ");
        Serial.println(err.c_str());
      }
    }
  }
}

void verifyEthernetStatus() {
  while (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet module not detected. Check connections.");
    delay(2000);
  }

  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    delay(2000);
  }
}
