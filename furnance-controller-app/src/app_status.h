#ifndef __APP_STATUS_H__
#define __APP_STATUS_H__


typedef enum
{
    STATUS_NO_ERROR          = 0,
    STATUS_NO_TEMP_SENSOR    = 1,
    STATUS_NO_RTC            = 2,
    STATUS_NO_EEPROM         = 3,
    STATUS_NO_DISPLAY        = 4,
    STATUS_TOO_HIGH_TEMP     = 5,
    STATUS_TOO_LOW_TEMP      = 6,
    STATUS_INVALID_TEMP      = 7,
    //------------------------------
    STATUS_LAST
} AppStatus;

static const char* app_status[AppStatus::STATUS_LAST] =
{
    "----",
    "ERR1",
    "ERR2",
    "ERR3",
    "ERR4",
    "ERR5",
    "ERR6",
    "ERR7",
};

#endif // __APP_STATUS_H__