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
    int set_time(const SystemTime& time);
    int get_time(SystemTime& time);
    float get_temperature();
    static char* time_string(const SystemTime& time);
    static char* date_string(const SystemTime& time);
    static char* time_date_string(const SystemTime& time);
    static const char* get_day_of_week_string(const SystemTime& time);
    static const char* get_month_string(const SystemTime& time);
    static uint8_t calculate_day_of_week(uint8_t d, uint8_t m, int y);
    static uint8_t validate_date(uint8_t d, uint8_t m, uint16_t y);

private:
    explicit SystemRtc();
    SystemRtc& operator=(const SystemRtc& rtc);

private:
    I2C i2c; // DS3231 i2c controller
};


#endif // __SYSTEM_RTC_H__
