#include "Rtc.h"
#include <Arduino.h>

Rtc::Rtc() : rtc() {}

void Rtc::init() {
  while (!rtc.begin()) {
    Serial.println(F("RTC initialization failed. Check wiring and power."));
    delay(2000);
  }

  Serial.println(F("RTC initialized successfully."));

  Serial.println("Setting date and time...");
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

String Rtc::getTime() {
  DateTime now = rtc.now();

  String date = String(now.year()) + "-";
  date += (now.month() < 10 ? "0" : "") + String(now.month()) + "-";
  date += (now.day() < 10 ? "0" : "") + String(now.day()) + "T";
  date += (now.hour() < 10 ? "0" : "") + String(now.hour()) + ":";
  date += (now.minute() < 10 ? "0" : "") + String(now.minute()) + ":";
  date += (now.second() < 10 ? "0" : "") + String(now.second()) + "-0600";

  return date;
}
