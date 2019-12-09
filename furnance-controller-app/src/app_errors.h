#ifndef __APP_ERRORS_H__
#define __APP_ERRORS_H__


typedef enum
{
    APP_ERROR_NO_ERROR,
    APP_ERROR_NO_TEMP_SENSOR,
    APP_ERROR_NO_RTC,
    APP_ERROR_NO_EEPROM,
    APP_ERROR_NO_DISPLAY,
    APP_ERROR_INVALID_TEMP,
    APP_ERROR_LAST
} AppError;

static const char* app_errors[AppError::APP_ERROR_LAST] =
{
    "ERR0",
    "ERR1",
    "ERR2",
    "ERR3",
    "ERR4",
};

#endif // __APP_ERRORS_H__