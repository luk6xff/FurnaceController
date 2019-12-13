#include "app.h"
#include "mbed.h"

#define DEBUG_ON 0

//------------------------------------------------------------------------------
App &App::instance()
{
    static App app;
    return app;
}

//------------------------------------------------------------------------------
App::App()
    : current_state(Init)
    , current_error(APP_ERROR_NO_ERROR)
    , tctrl(AppSettings::instance().get_defaults().temp)
    , watchdog(Watchdog::get_instance())
{
    debug_if(DEBUG_ON, "APP: --- Hello from furnance controller app! ---\r\n");
}

//------------------------------------------------------------------------------
void App::run()
{
    while(true)
    {
        switch(current_state)
        {
            case Init:
            {
                init();
                current_state = MainApp;
                break;
            }

            case MainApp:
            {
                int res = main_app();
                if (res == 1)
                {
                    debug_if(DEBUG_ON, "APP: Settings menu chosen\r\n");
                    current_state = Settings;
                }
                else if (res == 2)
                {
                    debug_if(DEBUG_ON, "APP: Error occured: ERR=[%d]\r\n", (int)get_error());
                    current_state = Error;
                }
                break;
            }

            case Settings:
            {
                settings_menu();
                current_state = MainApp;
                break;
            }

            case Error:
            {
                error_handler();
                current_state = MainApp;
                break;
            }

            default:
                break;
        }
    }
}

//------------------------------------------------------------------------------
void App::init()
{
    debug_if(DEBUG_ON, "APP: --- Hello from INIT ---\r\n");
    // Configure Watchdog
    watchdog.start(WATCHDOG_TIMEOUT_MS);
    watchdog_ticker.attach(mbed::callback(this, &App::ev_kick_watchdog), 5); // every 5 seconds
    // Start global timer
    app_timer.start();

    // Configure EEPROM Settings
    AppSettings::instance().init();
}

//------------------------------------------------------------------------------
int App::main_app()
{
    debug_if(DEBUG_ON, "APP: --- MAIN_APP ---\r\n");
    const uint8_t other_settings_timeout_s = 4; //[s]
    Ticker update_temp_ticker;
    update_temp_ticker.attach(mbed::callback(this, &App::ev_update_temp_ctrl), 1); // every 1 second

    Ticker update_time_ticker;
    update_time_ticker.attach(mbed::callback(this, &App::ev_update_time), 1); // every 1 second

    while(1)
    {
        const ButtonState ok_state = btns.check_button(BtnTypeOk);
        const ButtonState up_state = btns.check_button(BtnTypeUp);
        const ButtonState down_state = btns.check_button(BtnTypeDown);

        if (ok_state == ButtonState::BtnHold_1s)
        {
            debug_if(DEBUG_ON, "APP: ButtonType::BtnOk pressed\r\n");
            return 1;
        }

        else if (up_state == ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "APP: ButtonType::BtnUp pressed - Show time\r\n");
            uint32_t start = app_timer.read();
            while ((app_timer.read()-start) < other_settings_timeout_s)
            {
                if (tctrl.get_relay_status() == TempController::TEMP_CTRL_RELAY_ON)
                {
                    check_time(true);
                }
                else
                {
                    // Relay off, show furnance temperature
                    check_temp_ctrl(true);
                }
                wait_us(500000);
            }

        }

        else if (down_state == ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "APP: ButtonType::BtnDown pressed - Show time\r\n");
            uint32_t start = app_timer.read();
            while ((app_timer.read()-start) < other_settings_timeout_s)
            {
                check_indoor_temperature();
                wait_us(500000);
            }
        }

        // Run main process function, display temperature if relay is on
        check_temp_ctrl(tctrl.get_relay_status() == TempController::TEMP_CTRL_RELAY_ON);

        // Do not display time when relay is on
        check_time(tctrl.get_relay_status() == TempController::TEMP_CTRL_RELAY_OFF);

        if (get_error() != APP_ERROR_NO_ERROR)
        {
            return 2;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
int App::settings_menu()
{
    debug("APP: --- Hello from SETTINGS ---\r\n");
    enum SettingsType
    {
        TEMPERATURE = 0,
        TIME,
        LAST,
    };
    const char* menu[LAST] = {"TEMP", "CZAS"};
    SettingsType current_settings = TEMPERATURE;
    int curr_set = (int)current_settings;
    uint32_t start = app_timer.read();
    while ((app_timer.read()-start) < settings_timeout_s)
    {
        const ButtonState ok_state = btns.check_button(BtnTypeOk);
        const ButtonState up_state = btns.check_button(BtnTypeUp);
        const ButtonState down_state = btns.check_button(BtnTypeDown);
        current_settings = (SettingsType)curr_set;
        disp.print(menu[curr_set], 4);

        if (ok_state == ButtonState::BtnPressed)
        {
            start = app_timer.read();
            debug_if(DEBUG_ON, "APP: Buttons::ButtonType::BtnOk pressed\r\n");
            if (current_settings == TEMPERATURE)
            {
                AppSettings::instance().set_temperatures(btns, disp);
            }
            else if (current_settings == TIME)
            {
                AppSettings::instance().set_date_time(btns, disp);
            }
            return 1;
        }
        else if (down_state == ButtonState::BtnPressed)
        {
            start = app_timer.read();
            debug_if(DEBUG_ON, "APP: Buttons::ButtonType::BtnDown pressed\r\n");
            curr_set--;
            if (curr_set < 0)
            {
                curr_set = (int)LAST - 1;
            };
        }
        else if (up_state == ButtonState::BtnPressed)
        {
            start = app_timer.read();
            debug_if(DEBUG_ON, "APP: Buttons::ButtonType::BtnUp pressed\r\n");
            curr_set = (curr_set+1) % (int)LAST;
        }

        if (ok_state == ButtonState::BtnHold_5s)
        {
            break;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
void App::error_handler()
{
    // Display last error
    debug_if(DEBUG_ON, "APP: APP_ERROR: %s\r\n", app_errors[get_error()]);
    disp.print_error(get_error());
    set_error(AppError::APP_ERROR_NO_ERROR);
}

//------------------------------------------------------------------------------
void App::ev_update_temp_ctrl()
{
    debug_if(DEBUG_ON, "APP: ev_update_temp_ctrl\r\n");
    update_temp_ctrl = true;
}

//------------------------------------------------------------------------------
void App::ev_update_time()
{
    debug_if(DEBUG_ON, "APP: ev_update_time\r\n");
    update_time = true;
}

//------------------------------------------------------------------------------
void App::ev_kick_watchdog()
{
    watchdog.kick();
}

//------------------------------------------------------------------------------
void App::check_temp_ctrl(bool show_temp)
{
    if (update_temp_ctrl)
    {
        debug_if(DEBUG_ON, "APP: update_temp_ctrl\r\n");
        // Update temperatures
        tctrl.update_temp_thresholds(AppSettings::instance().get_current().temp);
        TempController::TempCtrlError status = tctrl.process();
        switch (status)
        {
            case TempController::TempCtrlError::TEMP_CTRL_NOERR:
                if (show_temp)
                {
                    debug_if(DEBUG_ON, "APP: LAST furnance temperature: %3.1f [C]\r\n", tctrl.get_last_temperature());
                    disp.print_temperature(tctrl.get_last_temperature());
                }
                break;

            case TempController::TempCtrlError::TEMP_CTRL_NO_SENSOR:
                set_error(APP_ERROR_NO_TEMP_SENSOR);
                break;

            case TempController::TempCtrlError::TEMP_CTRL_TEMP_TOO_LOW:
            case TempController::TempCtrlError::TEMP_CTRL_TEMP_TOO_HIGH:
                set_error(APP_ERROR_INVALID_TEMP);
                break;

            default:
                break;
        }
        update_temp_ctrl = false;
    }
}

//------------------------------------------------------------------------------
void App::check_time(bool show_time)
{
    if (update_time)
    {
        // No need to read time if it's not gonne be displayed
        if (show_time)
        {
            static bool blink_on = false;
            SystemTime current_time;
            int ret = SystemRtc::instance().get_time(current_time);
            if (ret > 0)
            {
                debug_if(DEBUG_ON, "APP: update_time - get_time error occured! ERR:%d\r\n", ret);
                set_error(APP_ERROR_NO_RTC);
            }
            else
            {
                debug_if(DEBUG_ON, "APP: Current time: %s\r\n", SystemRtc::time_date_string(current_time));
                blink_on = blink_on^1;
                disp.print_time(current_time.hours, current_time.minutes, blink_on);
            }
        }
        update_time = false;
    }
}

//------------------------------------------------------------------------------
void App::check_indoor_temperature()
{
    float temperature = SystemRtc::instance().get_temperature();
    debug_if(DEBUG_ON, "APP: Current indoor temperature: %3.1f\r\n", temperature);
    disp.print_temperature(temperature, TM1637_BRT3);
}

//------------------------------------------------------------------------------
void App::set_error(AppError err)
{
    current_error = err;
}

//------------------------------------------------------------------------------
AppError App::get_error() const
{
    return current_error;
}

//------------------------------------------------------------------------------