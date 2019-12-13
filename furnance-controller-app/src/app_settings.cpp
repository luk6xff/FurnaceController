#include "app_settings.h"
#include "hw_config.h"

#include "display.h"
#include "system_rtc.h"
#include "temp_controller.h"

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
            .temp_relay_off = 28,
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
bool AppSettings::set_date_time(Buttons& btns, Display& disp)
{
    SystemTime time;
    bool is_time_set = false;
    uint8_t h_or_m = 0;
    if (SystemRtc::instance().get_time(time) > 0)
    {
        // time.year = 2020;
        // time.month = 1;
        // time.day = 1;
        // time.seconds = 0;  NOT USED
        time.hours = 0;
        time.minutes = 0;

    }
    disp.print_time(time.hours, time.minutes, true);
    while (1)
    {
        const ButtonState ok_state = btns.check_button(BtnTypeOk);
        const ButtonState up_state = btns.check_button(BtnTypeUp);
        const ButtonState down_state = btns.check_button(BtnTypeDown);
        if (h_or_m == 0) // minutes
        {
            if (up_state == ButtonState::BtnPressed)
            {
                time.minutes = (time.minutes+1) % 60;
                disp.print_time(time.hours, time.minutes, true);
            }
            else if (down_state == ButtonState::BtnPressed)
            {
                if (time.minutes == 0)
                {
                    time.minutes = 59;
                }
                else
                {
                    time.minutes--;
                }

                disp.print_time(time.hours, time.minutes, true);
            }
            else if (ok_state == ButtonState::BtnPressed)
            {
                h_or_m = 1; // hours
            }
        }
        else if (h_or_m == 1)
        {
            if (up_state == ButtonState::BtnPressed)
            {
                time.hours = (time.hours+1) % 24;
                disp.print_time(time.hours, time.minutes, true);
            }
            else if (down_state == ButtonState::BtnPressed)
            {
                if (time.hours == 0)
                {
                    time.hours = 23;
                }
                else
                {
                    time.hours--;
                }
                disp.print_time(time.hours, time.minutes, true);
            }
            else if (ok_state == ButtonState::BtnPressed)
            {
                h_or_m = 1; // hours
            }
        }

        if (ok_state == ButtonState::BtnHold_5s)
        {
            is_time_set = true;
            break;
        }
        else if (ok_state == ButtonState::BtnHold_1s)
        {
            // Nothing
        }
    }

    disp.clear();
    if (is_time_set)
    {
        if (!SystemRtc::instance().set_time(time))
        {
            debug_if(DEBUG_ON, "APP_SETTINGS: New time: %d:%d saved correctly! \r\n", time.hours, time.minutes);
            disp.print("oooo", 4);
        }
        else
        {
            debug_if(DEBUG_ON, "APP_SETTINGS: New time: %d:%d has not been saved! \r\n", time.hours, time.minutes);
            disp.print("nnnn", 4);
            return false;
        }

    }
    wait_us(1000000);
    return true;
}

//------------------------------------------------------------------------------
bool AppSettings::set_temperatures(Buttons& btns, Display& disp)
{
    TempController::TempCtrlSettings temps = get_current().temp;
    bool is_temp_set = false;
    enum SettingsType
    {
        FIRST = -1,
        TEMP_RELAY_ON = 0,
        TEMP_RELAY_OFF,
        LAST,
    };
    const char* menu[LAST] = {"T_ON", "TOFF"};
    SettingsType curr_set = TEMP_RELAY_ON;

    while (1)
    {
        ButtonState ok_state = btns.check_button(BtnTypeOk);
        ButtonState up_state = btns.check_button(BtnTypeUp);
        ButtonState down_state = btns.check_button(BtnTypeDown);
        disp.print(menu[curr_set], 4);

        if (ok_state == ButtonState::BtnPressed)
        {
            if (curr_set == TEMP_RELAY_ON)
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: TEMP_RELAY_ON: %d chosen! \r\n", temps.temp_relay_on);
                disp.print_temperature(temps.temp_relay_on);
                while (1)
                {
                    ok_state = btns.check_button(BtnTypeOk);
                    up_state = btns.check_button(BtnTypeUp);
                    down_state = btns.check_button(BtnTypeDown);
                    if (up_state == ButtonState::BtnPressed)
                    {
                        temps.temp_relay_on++;
                        if (temps.temp_relay_on > temps.temp_max)
                        {
                            temps.temp_relay_on = temps.temp_min;
                        }
                        disp.print_temperature(temps.temp_relay_on);
                    }
                    else if (down_state == ButtonState::BtnPressed)
                    {
                        temps.temp_relay_on--;
                        if (temps.temp_relay_on < temps.temp_min)
                        {
                            temps.temp_relay_on = temps.temp_max;
                        }
                        disp.print_temperature(temps.temp_relay_on);
                    }
                    if (ok_state == ButtonState::BtnHold_1s)
                    {
                        is_temp_set = true;
                        break;
                    }
                }
            }
            else if (curr_set == TEMP_RELAY_OFF)
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: TEMP_RELAY_OFF: %d chosen! \r\n", temps.temp_relay_off);
                disp.print_temperature(temps.temp_relay_off);
                while (1)
                {
                    ok_state = btns.check_button(BtnTypeOk);
                    up_state = btns.check_button(BtnTypeUp);
                    down_state = btns.check_button(BtnTypeDown);
                    if (up_state == ButtonState::BtnPressed)
                    {
                        temps.temp_relay_off++;
                        if (temps.temp_relay_off > temps.temp_relay_on)
                        {
                            temps.temp_relay_off = temps.temp_min;
                        }
                        disp.print_temperature(temps.temp_relay_off);
                    }
                    else if (down_state == ButtonState::BtnPressed)
                    {
                        temps.temp_relay_off--;
                        if (temps.temp_relay_off < temps.temp_min)
                        {
                            temps.temp_relay_off = temps.temp_relay_on;
                        }
                        disp.print_temperature(temps.temp_relay_off);
                    }
                    if (ok_state == ButtonState::BtnHold_1s)
                    {
                        is_temp_set = true;
                        break;
                    }
                }
            }
            if (is_temp_set)
            {
                break;
            }
        }

        else if (up_state == ButtonState::BtnPressed)
        {
            curr_set = (SettingsType)(((int)curr_set+1)%(int)LAST);
        }

        else if (down_state == ButtonState::BtnPressed)
        {
            if ((int)curr_set < 0)
            {
               curr_set = (SettingsType)((int)LAST-1);
            }
            curr_set = (SettingsType)((int)curr_set-1);

        }
    }

    disp.clear();
    if (is_temp_set)
    {
        Settings current = get_current();
        current.temp = temps;
        if (!save_settings(current))
        {
            debug_if(DEBUG_ON, "APP_SETTINGS: Updating eeprom with new temperatures failed!\r\n");
            current_settings = default_settings;
            disp.print("nnnn", 4);

        }
        else
        {
            debug_if(DEBUG_ON, "APP_SETTINGS: Updating eeprom with new temperatures succeed!\r\n");
            disp.print("oooo", 4);
        }
    }
    wait_us(1000000);
    return true;
}

//------------------------------------------------------------------------------