#include "app_settings.h"


//------------------------------------------------------------------------------
AppSettings::AppSettings()
{
    // Modify according to your application
    default_settings =
    {
        .magic = 0x4C554B36,  // LUK6
        .version = 0x00000001,
        .temp =
        {
            .temp_min = -20,
            .temp_max = 120,
            .temp_relay_on = 30,
            .temp_hysteresis = 3,
        },
    };
}


//------------------------------------------------------------------------------
AppSettings&  AppSettings::instance()
{
    static AppSettings settings;
    return settings;
}


//------------------------------------------------------------------------------
const AppSettings::Settings& AppSettings::get_defaults()
{
    return default_settings;
}

//------------------------------------------------------------------------------