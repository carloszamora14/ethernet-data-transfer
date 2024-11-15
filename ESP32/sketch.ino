#include <SPI.h>
#include <UIPEthernet.h>

// Network Settings
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEE};
IPAddress esp32IP(192, 168, 0, 166);
IPAddress arduinoIP(192, 168, 0, 177);
unsigned int udpPort = 8888;

EthernetUDP udp;
char receivedMessage[50];

void setup() {
  Serial.begin(115200);

  Ethernet.begin(mac, esp32IP);
  Serial.print("Assigned IP Address: ");
  Serial.println(Ethernet.localIP());

  udp.begin(udpPort);
  Serial.println("ESP32 Ethernet UDP Initialized");
}

void loop() {
  receiveMessageFromArduino();
  sendMessageToArduino("Hello from ESP32");
  delay(2000);
}

void receiveMessageFromArduino() {
  int packetSize = udp.parsePacket();
  if (packetSize) {
    int len = udp.read(receivedMessage, 50);
    if (len > 0)
      receivedMessage[len] = '\0';
    Serial.print("Received from Arduino: ");
    Serial.println(receivedMessage);

    char newMessage[100];
    snprintf(newMessage, sizeof(newMessage), "%s time2:%d", receivedMessage, millis());

    Serial.print("New message with millis: ");
    Serial.println(newMessage);

    sendMessageToArduino(newMessage);
  } else {
    Serial.println("No packet received.");
  }
}

void sendMessageToArduino(const char *message) {
  udp.beginPacket(arduinoIP, udpPort);
  if (udp.print(message) > 0) {
    Serial.print("Message sent to Arduino: ");
    Serial.println(message);
  } else {
    Serial.println("Failed to send message.");
  }
  udp.endPacket();
}
