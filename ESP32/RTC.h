#ifndef RTC_H
#define RTC_H

#include <Wire.h>
#include "RTClib.h"

class Rtc {
public:
  Rtc();
  void init();
  String getTime();

private:
  RTC_DS1307 rtc;
};

#endif
