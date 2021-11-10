#include "app.h"
#include "mbed.h"

#define DEBUG_ON 1

//------------------------------------------------------------------------------
#define OPTIONAL_PARAM_TIMEOUT_S      5 // [s]
#define UPDATE_PARAM_TICKER_PERIOD_S  1 // every 1 second
#define TEMPERATURE_UPDATE_PERIOD_S   2 // every 2 seconds
#define TIME_UPDATE_PERIOD_S          1 // every 1 second
#define WATCHDOG_TIMEOUT_MS           10000 // 10 seconds
#define WATCHDOG_KICKER_PERIOD_S      6 // every 6 seconds

//------------------------------------------------------------------------------
App &App::instance()
{
    static App app;
    return app;
}

//------------------------------------------------------------------------------
App::App()
    : current_state(Init)
    , current_status(STATUS_NO_ERROR)
    , settings(app_timer)
    , tctrl(settings.get_defaults().temp)
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
                current_state = main_app();
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
    // Configure Watchdog
    watchdog.start(WATCHDOG_TIMEOUT_MS);
    watchdog_ticker.attach(mbed::callback(this, &App::ev_kick_watchdog),
                                          WATCHDOG_KICKER_PERIOD_S);
    // Start global timer
    app_timer.start();

    // Configure EEPROM Settings
    settings.init();
}

//------------------------------------------------------------------------------
App::AppState App::main_app()
{
    update_param_ticker.attach(mbed::callback(this, &App::ev_update_param),
                                             UPDATE_PARAM_TICKER_PERIOD_S);

    while(1)
    {
        const ButtonState ok_state = btns.check_button(BtnTypeOk);
        const ButtonState up_state = btns.check_button(BtnTypeUp);
        const ButtonState down_state = btns.check_button(BtnTypeDown);

        if (ok_state == ButtonState::BtnHold_1s)
        {
            return App::AppState::Settings;
        }

        else if (up_state == ButtonState::BtnPressed)
        {
            uint32_t start = app_timer.read();
            while ((app_timer.read()-start) < OPTIONAL_PARAM_TIMEOUT_S)
            {
                if (tctrl.get_relay_status() == TempController::TEMP_CTRL_RELAY_ON)
                {
                    check_time(true);
                }
                else
                {
                    // Relay off, only show furnance temperature
                    disp.print_temperature(tctrl.get_last_valid_temperature());
                }
                wait_us(500000);
            }
        }

        else if (down_state == ButtonState::BtnPressed)
        {
            uint32_t start = app_timer.read();
            while ((app_timer.read()-start) < OPTIONAL_PARAM_TIMEOUT_S)
            {
                check_indoor_temperature();
                wait_us(500000);
            }
        }

        if (tctrl.get_relay_status() == TempController::TEMP_CTRL_RELAY_OFF)
        {
            // Display time when relay is off
            check_time(true);
            // Run main process function, don't display temperature if relay is off
            check_temp_ctrl(false);
        }
        else
        {
            // Run main process function, don't display temperature if relay is on
            check_temp_ctrl(true);
        }

        if (get_status() != STATUS_NO_ERROR)
        {
            return App::AppState::Error;
        }
    }

    return App::AppState::MainApp;
}

//------------------------------------------------------------------------------
void App::settings_menu()
{
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
        disp.print_str(menu[curr_set], 4);

        if (ok_state == ButtonState::BtnPressed)
        {
            start = app_timer.read();
            if (current_settings == TEMPERATURE)
            {
                settings.set_temperatures(btns, disp);
            }
            else if (current_settings == TIME)
            {
                settings.set_date_time(btns, disp);
            }
            return;
        }
        else if (down_state == ButtonState::BtnPressed)
        {
            start = app_timer.read();
            curr_set--;
            if (curr_set < 0)
            {
                curr_set = (int)LAST - 1;
            };
        }
        else if (up_state == ButtonState::BtnPressed)
        {
            start = app_timer.read();
            curr_set = (curr_set+1) % (int)LAST;
        }

        if (ok_state == ButtonState::BtnHold_5s)
        {
            break;
        }
    }
}

//------------------------------------------------------------------------------
void App::error_handler()
{
    // Display last error
    debug_if(DEBUG_ON, "APP: APP_ERROR: %s\r\n", app_status[get_status()]);
    disp.print_str(app_status[get_status()], 4);
    wait_us(1000000);
    set_status(AppStatus::STATUS_NO_ERROR);
}

//------------------------------------------------------------------------------
void App::ev_update_param()
{
    debug_if(DEBUG_ON, "APP: ev_update_param\r\n");
    static uint32_t counter = 0;
    counter++;

    if ((counter % TEMPERATURE_UPDATE_PERIOD_S) == 0)
    {
        update_temp_ctrl = true;
    }
    if ((counter % TIME_UPDATE_PERIOD_S) == 0)
    {
        update_time = true;
    }
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
        tctrl.update_temp_thresholds(settings.get_current().temp);
        const AppStatus status = tctrl.process();
        set_status(status);
        if (status == AppStatus::STATUS_NO_ERROR && show_temp)
        {
            debug_if(DEBUG_ON, "APP: LAST furnance temperature: %3.1f [C]\r\n", tctrl.get_last_valid_temperature());
            disp.print_temperature(tctrl.get_last_valid_temperature());
        }
        update_temp_ctrl = false;
    }
}

//------------------------------------------------------------------------------
void App::check_time(bool show_time)
{
    // No need to read time if it's not gonne be displayed
    if (update_time && show_time)
    {
        static bool blink_on = false;
        static SystemTime current_time;
        int ret = SystemRtc::instance().get_time(current_time);
        if (ret > 0)
        {
            set_status(STATUS_NO_RTC);
        }
        else
        {
            blink_on = blink_on^1;
            disp.print_time(current_time.hours, current_time.minutes, blink_on);
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
void App::set_status(AppStatus err)
{
    current_status = err;
}

//------------------------------------------------------------------------------
AppStatus App::get_status() const
{
    return current_status;
}

//------------------------------------------------------------------------------