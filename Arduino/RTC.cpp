#include "RTC.h"
#include <Arduino.h>

RTC::RTC() : rtc() {}

void RTC::init() {
  if (!rtc.begin()) {
    Serial.println("RTC initialization failed. Check wiring and power.");
    while (true) {
      delay(1000);
    }
  } else {
    Serial.println("RTC initialized successfully.");
  }

  if (!rtc.isrunning()) {
    Serial.println("RTC is not running. Setting date and time...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  } else {
    Serial.println("RTC is running.");
  }
}

String RTC::getTime() {
  DateTime now = rtc.now();

  String date = String(now.year()) + "-";
  date += (now.month() < 10 ? "0" : "") + String(now.month()) + "-";
  date += (now.day() < 10 ? "0" : "") + String(now.day()) + "T";
  date += (now.hour() < 10 ? "0" : "") + String(now.hour()) + ":";
  date += (now.minute() < 10 ? "0" : "") + String(now.minute()) + ":";
  date += (now.second() < 10 ? "0" : "") + String(now.second()) + "Z";

  return date;
}
