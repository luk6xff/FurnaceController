#ifndef __SYSTEM_RTC_H__
#define __SYSTEM_RTC_H__

#include "../lib/DS3231/platform/mbed/ds3231-mbed.h"

struct SystemTime
{
    uint16_t year;
    uint8_t month;
    uint8_t date;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
};

class SystemRtc
{
public:
    static SystemRtc& instance();
    bool set_time(const SystemTime& time);
    bool get_time(SystemTime& time);
    float get_temperature();
    static char* time_string(const SystemTime& time);
    static char* date_string(const SystemTime& time);
    static char* time_date_string(const SystemTime& time);
    static const char* get_day_of_week_string(const SystemTime& time);
    static const char* get_month_string(const SystemTime& time);

private:
    explicit SystemRtc();
    SystemRtc& operator=(const SystemRtc& rtc);
};


#endif // __SYSTEM_RTC_H__