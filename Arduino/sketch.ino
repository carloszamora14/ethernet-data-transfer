#include <SPI.h>
#include <UIPEthernet.h>

// Network Settings
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress arduinoIP(192, 168, 0, 177);
IPAddress remoteIP(192, 168, 0, 166);
unsigned int udpPort = 8888;

EthernetUDP udp;
char receivedMessage[100];
int id = 0;

void setup() {
  Serial.begin(9600);

  Ethernet.begin(mac, arduinoIP);
  Serial.print("Assigned IP Address: ");
  Serial.println(Ethernet.localIP());

  udp.begin(udpPort);
  Serial.println("Arduino Ethernet UDP Initialized");

  delay(1000);
}

void loop() {
  sendMessage();
  receiveMessageFromESP32();
  delay(1000);
}

void receiveMessageFromESP32() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(receivedMessage, 100);
    if (len > 0)
      receivedMessage[len] = '\0';
    Serial.print("Received from ESP32: ");
    Serial.println(receivedMessage);
  } else {
    Serial.println("No packet received.");
  }
}

void sendMessage() {
  char message[100];
  snprintf(message, sizeof(message), "id:%d time1:%d", id++, millis());

  // Try to send the message to the remote IP and port
  udp.beginPacket(remoteIP, udpPort);
  if (udp.print(message) > 0) {
    Serial.print("Message sent: ");
    Serial.println(message);
  } else {
    Serial.println("Failed to send message.");
  }
  udp.endPacket();
}
