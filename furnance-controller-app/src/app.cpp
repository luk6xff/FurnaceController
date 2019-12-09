#include "app.h"
#include "mbed.h"

#define DEBUG_ON 1

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
                    debug_if(DEBUG_ON, "APP: Setting menu chosen\r\n");
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

    // Configure EEPROM Settings
    AppSettings::instance().init();
}

//------------------------------------------------------------------------------
int App::main_app()
{
    debug_if(DEBUG_ON, "APP: --- Hello from MAIN_APP ---\r\n");
    Ticker update_temp_ticker;
    update_temp_ticker.attach(mbed::callback(this, &App::ev_update_temp_ctrl), 1); // every 1 second

    Ticker update_time_ticker;
    update_time_ticker.attach(mbed::callback(this, &App::ev_update_time), 1); // every 1 second

    while(1)
    {
        Buttons::BtnInfo info = btns.check_buttons();
        if (info.state[Buttons::ButtonType::BtnOk] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "APP: Buttons::ButtonType::BtnOk pressed\r\n");
            return 1;
        }
        else if (info.state[Buttons::ButtonType::BtnLeft] == Buttons::ButtonState::BtnPressed ||
                 info.state[Buttons::ButtonType::BtnRight] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "APP: Buttons::ButtonType::BtnLeft/BtnRight pressed\r\n");
        }
        check_temp_ctrl();
        check_time();

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
    while(1)
    {
        Buttons::BtnInfo info = btns.check_buttons();
        if (info.state[Buttons::ButtonType::BtnOk] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "APP: Buttons::ButtonType::BtnOk pressed\r\n");
            return 1;
        }
        else if (info.state[Buttons::ButtonType::BtnLeft] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "APP: Buttons::ButtonType::BtnLeft pressed\r\n");
            return 2;
        }
        else if (info.state[Buttons::ButtonType::BtnRight] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "APP: Buttons::ButtonType::BtnRight pressed\r\n");
            return 3;
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
void App::check_temp_ctrl()
{
    if (update_temp_ctrl)
    {
        debug_if(DEBUG_ON, "APP: update_temp_ctrl set ON\r\n");
        TempController::TempCtrlError status = tctrl.process();
        switch (status)
        {
            case TempController::TempCtrlError::TEMP_CTRL_NOERR:
                debug_if(DEBUG_ON, "APP: LAST temperature: %3.1f [C]\r\n", tctrl.get_last_temperature());
                disp.print_temperature(tctrl.get_last_temperature());
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
        wait(1);
    }
}

//------------------------------------------------------------------------------
void App::check_time()
{
    if (update_time)
    {
        static bool blink_on = false;
        SystemTime current_time;
        int ret = SystemRtc::instance().get_time(current_time);
        if (ret > 0)
        {
            debug_if(DEBUG_ON, "APP: update_time - get_time error occured! ERR:%d\r\n", ret);
        }
        else
        {
            debug_if(DEBUG_ON, "APP: Current time: %s\r\n", SystemRtc::time_date_string(current_time));
            blink_on = blink_on^1;
            disp.print_time(current_time.hours, current_time.minutes, blink_on);
        }
        update_time = false;
    }
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