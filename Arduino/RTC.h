#ifndef RTC_H
#define RTC_H

#include "RTClib.h"
#include <Wire.h>

class Rtc {
  public:
    Rtc();
    void init();
    String getTime();

  private:
    RTC_DS1307 rtc;
};

#endif
