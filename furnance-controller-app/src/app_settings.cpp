#include "app_settings.h"
#include "hw_config.h"

#include "display.h"
#include "system_rtc.h"
#include "temp_controller.h"

#define DEBUG_ON 0


//------------------------------------------------------------------------------
AppSettings::AppSettings(Timer& timer)
    : t(timer)
    , at24c32_i2c(AT24CXX_SDA, AT24CXX_SCL)
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
        .version = 0x00000003,
        .temp =
        {
            .temp_min = 5,
            .temp_max = 80,
            .temp_relay_on = 30,
            .temp_relay_off = 28,
            .temp_diff_valid = 20,
            .num_of_invalid_measurements_to_err = 10,
        },
    };
}

//------------------------------------------------------------------------------
void AppSettings::init()
{
    if (read_rettings())
    {
        // Print content
        print_current_settings();

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
void AppSettings::print_current_settings()
{
    debug_if(DEBUG_ON, "APP_SETTINGS: <<CURRENT APP SETTINGS>>\r\n");
    debug_if(DEBUG_ON, "magic: 0x%08x\r\n", get_current().magic);
    debug_if(DEBUG_ON, "version: 0x%08x\r\n", get_current().version);
    debug_if(DEBUG_ON, "temp.temp_min: %d\r\n", get_current().temp.temp_min);
    debug_if(DEBUG_ON, "temp.temp_max: %d\r\n", get_current().temp.temp_max);
    debug_if(DEBUG_ON, "temp.temp_relay_on: %d\r\n", get_current().temp.temp_relay_on);
    debug_if(DEBUG_ON, "temp.temp_relay_off: %d\r\n", get_current().temp.temp_relay_off);
    debug_if(DEBUG_ON, "APP_SETTINGS: <<CURRENT APP SETTINGS>>\r\n\r\n");
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
    disp.print_time(time.hours, time.minutes, true, true);
    // Set timeout
    uint32_t start = t.read();
    while ((t.read()-start) < settings_timeout_s)
    {
        const ButtonState ok_state = btns.check_button(BtnTypeOk);
        const ButtonState up_state = btns.check_button(BtnTypeUp);
        const ButtonState down_state = btns.check_button(BtnTypeDown);
        if (h_or_m == 0) // minutes
        {
            if (up_state == ButtonState::BtnPressed)
            {
                start = t.read();
                time.minutes = (time.minutes+1) % 60;
                disp.print_time(time.hours, time.minutes, true, true);
            }
            else if (down_state == ButtonState::BtnPressed)
            {
                start = t.read();
                if (time.minutes == 0)
                {
                    time.minutes = 59;
                }
                else
                {
                    time.minutes--;
                }

                disp.print_time(time.hours, time.minutes, true, true);
            }
            else if (ok_state == ButtonState::BtnPressed)
            {
                start = t.read();
                h_or_m = 1; // hours
            }
        }
        else if (h_or_m == 1)
        {
            if (up_state == ButtonState::BtnPressed)
            {
                start = t.read();
                time.hours = (time.hours+1) % 24;
                disp.print_time(time.hours, time.minutes, true, true);
            }
            else if (down_state == ButtonState::BtnPressed)
            {
                start = t.read();
                if (time.hours == 0)
                {
                    time.hours = 23;
                }
                else
                {
                    time.hours--;
                }
                disp.print_time(time.hours, time.minutes, true, true);
            }
            else if (ok_state == ButtonState::BtnPressed)
            {
                start = t.read();
                h_or_m = 1; // hours
            }
        }

        if (ok_state == ButtonState::BtnHold_5s)
        {
            start = t.read();
            is_time_set = true;
            break;
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
        TEMP_RELAY_ON = 0,
        TEMP_RELAY_OFF,
        TEMP_MIN,
        TEMP_MAX,
        TEMP_DIFF_VALID,
        TEMP_INVALID_MEAS_NUM,
        LAST,
    };
    const char* menu[LAST] = {"T_ON", "TOFF", "TMIN", "TMAX", "TDIF", "NinM"};
    SettingsType curr_set = TEMP_RELAY_ON;

    uint32_t start = t.read();
    while ((t.read()-start) < settings_timeout_s)
    {
        ButtonState ok_state = btns.check_button(BtnTypeOk);
        ButtonState up_state = btns.check_button(BtnTypeUp);
        ButtonState down_state = btns.check_button(BtnTypeDown);
        disp.print_str(menu[curr_set], 4);

        if (ok_state == ButtonState::BtnPressed)
        {
            start = t.read();
            if (curr_set == TEMP_RELAY_ON)
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: TEMP_RELAY_ON: %d chosen! \r\n", temps.temp_relay_on);
                disp.print_temperature(temps.temp_relay_on, TM1637_BRT6, true);
                while ((t.read()-start) < settings_timeout_s)
                {
                    ok_state = btns.check_button(BtnTypeOk);
                    up_state = btns.check_button(BtnTypeUp);
                    down_state = btns.check_button(BtnTypeDown);
                    if (up_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_relay_on++;
                        if (temps.temp_relay_on > temps.temp_max)
                        {
                            temps.temp_relay_on = temps.temp_relay_off;
                        }
                        disp.print_temperature(temps.temp_relay_on, TM1637_BRT6, true);
                    }
                    else if (down_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_relay_on--;
                        if (temps.temp_relay_on < temps.temp_relay_off)
                        {
                            temps.temp_relay_on = temps.temp_max;
                        }
                        disp.print_temperature(temps.temp_relay_on, TM1637_BRT6, true);
                    }
                    if (ok_state == ButtonState::BtnHold_5s)
                    {
                        start = t.read();
                        is_temp_set = true;
                        break;
                    }
                }
            }
            else if (curr_set == TEMP_RELAY_OFF)
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: TEMP_RELAY_OFF: %d chosen! \r\n", temps.temp_relay_off);
                disp.print_temperature(temps.temp_relay_off);
                while ((t.read()-start) < settings_timeout_s)
                {
                    ok_state = btns.check_button(BtnTypeOk);
                    up_state = btns.check_button(BtnTypeUp);
                    down_state = btns.check_button(BtnTypeDown);
                    if (up_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_relay_off++;
                        if (temps.temp_relay_off > temps.temp_relay_on)
                        {
                            temps.temp_relay_off = temps.temp_min;
                        }
                        disp.print_temperature(temps.temp_relay_off, TM1637_BRT6, true);
                    }
                    else if (down_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_relay_off--;
                        if (temps.temp_relay_off < temps.temp_min)
                        {
                            temps.temp_relay_off = temps.temp_relay_on;
                        }
                        disp.print_temperature(temps.temp_relay_off, TM1637_BRT6, true);
                    }
                    if (ok_state == ButtonState::BtnHold_5s)
                    {
                        start = t.read();
                        is_temp_set = true;
                        break;
                    }
                }
            }
            else if (curr_set == TEMP_MIN)
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: TEMP_MIN: %d chosen! \r\n", temps.temp_min);
                disp.print_temperature(temps.temp_min, TM1637_BRT6, true);
                while ((t.read()-start) < settings_timeout_s)
                {
                    ok_state = btns.check_button(BtnTypeOk);
                    up_state = btns.check_button(BtnTypeUp);
                    down_state = btns.check_button(BtnTypeDown);
                    if (up_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_min++;
                        if (temps.temp_min >= temps.temp_max)
                        {
                            temps.temp_min = k_temperature_min;
                        }
                        disp.print_temperature(temps.temp_min, TM1637_BRT6, true);
                    }
                    else if (down_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_min--;
                        if (temps.temp_min < k_temperature_min)
                        {
                            temps.temp_min = temps.temp_max-1;
                        }
                        disp.print_temperature(temps.temp_min, TM1637_BRT6, true);
                    }
                    if (ok_state == ButtonState::BtnHold_5s)
                    {
                        start = t.read();
                        is_temp_set = true;
                        break;
                    }
                }
            }
            else if (curr_set == TEMP_MAX)
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: TEMP_MAX: %d chosen! \r\n", temps.temp_max);
                disp.print_temperature(temps.temp_max, TM1637_BRT6, true);
                while ((t.read()-start) < settings_timeout_s)
                {
                    ok_state = btns.check_button(BtnTypeOk);
                    up_state = btns.check_button(BtnTypeUp);
                    down_state = btns.check_button(BtnTypeDown);
                    if (up_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_max++;
                        if (temps.temp_max >= k_temperature_max-1)
                        {
                            temps.temp_max = temps.temp_min+1;
                        }
                        disp.print_temperature(temps.temp_max, TM1637_BRT6, true);
                    }
                    else if (down_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_max--;
                        if (temps.temp_max <= temps.temp_min)
                        {
                            temps.temp_max = k_temperature_max;
                        }
                        disp.print_temperature(temps.temp_max, TM1637_BRT6, true);
                    }
                    if (ok_state == ButtonState::BtnHold_5s)
                    {
                        start = t.read();
                        is_temp_set = true;
                        break;
                    }
                }
            }
            else if (curr_set == TEMP_DIFF_VALID)
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: TEMP_DIFF_VALID: %d chosen! \r\n", temps.temp_diff_valid);
                disp.print_temperature(temps.temp_diff_valid, TM1637_BRT6, true);
                while ((t.read()-start) < settings_timeout_s)
                {
                    ok_state = btns.check_button(BtnTypeOk);
                    up_state = btns.check_button(BtnTypeUp);
                    down_state = btns.check_button(BtnTypeDown);
                    if (up_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_diff_valid++;
                        if (temps.temp_diff_valid > 100)
                        {
                            temps.temp_diff_valid = 0;
                        }
                        disp.print_temperature(temps.temp_diff_valid, TM1637_BRT6, true);
                    }
                    else if (down_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.temp_diff_valid--;
                        if (temps.temp_diff_valid <= 0)
                        {
                            temps.temp_diff_valid = 100;
                        }
                        disp.print_temperature(temps.temp_diff_valid, TM1637_BRT6, true);
                    }
                    if (ok_state == ButtonState::BtnHold_5s)
                    {
                        start = t.read();
                        is_temp_set = true;
                        break;
                    }
                }
            }
            else if (curr_set == TEMP_INVALID_MEAS_NUM)
            {
                debug_if(DEBUG_ON, "APP_SETTINGS: TEMP_INVALID_MEAS_NUM: %d chosen! \r\n", temps.num_of_invalid_measurements_to_err);
                disp.print_dec(temps.num_of_invalid_measurements_to_err, true);
                while ((t.read()-start) < settings_timeout_s)
                {
                    ok_state = btns.check_button(BtnTypeOk);
                    up_state = btns.check_button(BtnTypeUp);
                    down_state = btns.check_button(BtnTypeDown);
                    if (up_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.num_of_invalid_measurements_to_err++;
                        if (temps.num_of_invalid_measurements_to_err > 100)
                        {
                            temps.num_of_invalid_measurements_to_err = 1;
                        }
                        disp.print_dec(temps.num_of_invalid_measurements_to_err, true);
                    }
                    else if (down_state == ButtonState::BtnPressed)
                    {
                        start = t.read();
                        temps.num_of_invalid_measurements_to_err--;
                        if (temps.num_of_invalid_measurements_to_err <= 1)
                        {
                            temps.num_of_invalid_measurements_to_err = 100;
                        }
                        disp.print_dec(temps.num_of_invalid_measurements_to_err, true);
                    }
                    if (ok_state == ButtonState::BtnHold_5s)
                    {
                        start = t.read();
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
            start = t.read();
            curr_set = (SettingsType)(((int)curr_set+1)%(int)LAST);
        }

        else if (down_state == ButtonState::BtnPressed)
        {
            start = t.read();
            if ((int)curr_set == 0)
            {
               curr_set = (SettingsType)((int)LAST);
            }
            curr_set = (SettingsType)((int)curr_set-1);
        }
    }

    disp.clear();
    if (is_temp_set)
    {
        current_settings.temp = temps;
        if (!save_settings(get_current()))
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
        wait_us(1000000);
        return true;
    }

    disp.print("nnnn", 4);
    wait_us(1000000);
    return true;

}

//------------------------------------------------------------------------------