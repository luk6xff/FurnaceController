#include "app_settings.h"
#include "hw_config.h"

//------------------------------------------------------------------------------
AppSettings::AppSettings()
    : at24c32_i2c(AT24CXX_SDA, AT24CXX_SCL)
    , at24c32_wp(AT24CXX_WP)
    , at24c32_mbed{&at24c32_i2c, &at24c32_wp}
{
    at24c32 =
    {
        .type = AT24C32,
        .addr = AT24CXX_I2C_ADDR,
    };
    at24cxx_mbed_init(&at24c32, &at24c32_mbed);

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