#ifndef RTC_H
#define RTC_H

#include "RTClib.h"
#include <Wire.h>

class RTC {
  public:
    RTC();
    void init();
    String getTime();

  private:
    RTC_DS1307 rtc;
};

#endif
