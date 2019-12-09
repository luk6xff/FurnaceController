#include "app_settings.h"
#include "hw_config.h"

#define DEBUG_ON 1

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
void AppSettings::init()
{
    if (read_rettings())
    {
        // Check if settings are valid
        if (get_current().magic == get_defaults().magic && \
            get_current().version == get_defaults().version)
        {
            debug_if(DEBUG_ON, "APP_SETTINGS: Read eeprom settings look ok\r\n");
            // Check if values are ok
        }
        else
        {
            debug_if(DEBUG_ON, "APP_SETTINGS: Read eeprom settings are invalid, updating with defaults\r\n");
            if (!save_settings(get_defaults()))
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: Updating eeprom with defaults failed!, setting current as defaults\r\n");
                current_settings = default_settings;
            }
            else
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: Updating eeprom with defaults succeed!\r\n");
            }
        }
    }
    else
    {
        debug_if(DEBUG_ON, "APP_SETTINGS: Read eeprom settings failed!, setting current as defaults\r\n");
        current_settings = default_settings;
    }
}

//------------------------------------------------------------------------------
const AppSettings::Settings& AppSettings::get_defaults()
{
    return default_settings;
}

//------------------------------------------------------------------------------
const AppSettings::Settings& AppSettings::get_current()
{
    return current_settings;
}

//------------------------------------------------------------------------------
bool AppSettings::save_settings(const Settings &settings)
{
    if (at24cxx_write(&at24c32, 0, (const uint8_t*)&settings, sizeof(Settings)) != AT24CXX_NOERR)
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
bool AppSettings::read_rettings()
{
    if (at24cxx_read(&at24c32, 0, (uint8_t*)&current_settings, sizeof(Settings)) != AT24CXX_NOERR)
    {
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------