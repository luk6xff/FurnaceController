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
    , tctrl(AppSettings::instance().get_defaults().temp)
    , watchdog(Watchdog::get_instance())
{
    debug_if(DEBUG_ON, "--- Hello from furnance controller app! ---\r\n");
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
                    debug_if(DEBUG_ON, "Setting menu chosen\r\n");
                    current_state = Settings;
                }
                break;
            }

            case Settings:
            {
                settings_menu();
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
    debug_if(DEBUG_ON, "--- Hello from INIT ---\r\n");
    // Configure Watchdog
    watchdog.start(WATCHDOG_TIMEOUT_MS);
    watchdog_ticker.attach(mbed::callback(this, &App::ev_kick_watchdog), 5); // every 5 seconds

    // Configure EEPROM Settings
    AppSettings::instance().init();
}

//------------------------------------------------------------------------------
int App::main_app()
{
    debug_if(DEBUG_ON, "--- Hello from MAIN_APP ---\r\n");
    Ticker update_temp_ticker;
    update_temp_ticker.attach(mbed::callback(this, &App::ev_update_temp_ctrl), 1); // every 1 second

    Ticker update_time_ticker;
    update_time_ticker.attach(mbed::callback(this, &App::ev_update_time), 1); // every 1 second

    while(1)
    {
        Buttons::BtnInfo info = btns.check_buttons();
        if (info.state[Buttons::ButtonType::BtnOk] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "Buttons::ButtonType::BtnOk pressed\r\n");
            return 1;
        }
        else if (info.state[Buttons::ButtonType::BtnLeft] == Buttons::ButtonState::BtnPressed ||
                 info.state[Buttons::ButtonType::BtnRight] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "Buttons::ButtonType::BtnLeft/BtnRight pressed\r\n");
        }
        check_temp_ctrl();
        check_time();
    }
    return 0;
}

//------------------------------------------------------------------------------
int App::settings_menu()
{
    debug("--- Hello from SETTINGS ---\r\n");
    while(1)
    {
        Buttons::BtnInfo info = btns.check_buttons();
        if (info.state[Buttons::ButtonType::BtnOk] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "Buttons::ButtonType::BtnOk pressed\r\n");
            return 1;
        }
        else if (info.state[Buttons::ButtonType::BtnLeft] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "Buttons::ButtonType::BtnLeft pressed\r\n");
            return 2;
        }
        else if (info.state[Buttons::ButtonType::BtnRight] == Buttons::ButtonState::BtnPressed)
        {
            debug_if(DEBUG_ON, "Buttons::ButtonType::BtnRight pressed\r\n");
            return 3;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
void App::ev_update_temp_ctrl()
{
    debug_if(DEBUG_ON, "ev_update_temp_ctrl\r\n");
    update_temp_ctrl = true;
}

//------------------------------------------------------------------------------
void App::ev_update_time()
{
    debug_if(DEBUG_ON, "ev_update_time\r\n");
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
        debug_if(DEBUG_ON, "update_temp_ctrl set ON\r\n");
        tctrl.process();
        debug_if(DEBUG_ON, "LAST temperature: %3.1f [C]\r\n", tctrl.get_last_temperature());
        disp.print_temperature(tctrl.get_last_temperature());
        // disp.print_temperature(-23.1f);
        // wait_ms(1000);
        // disp.print_temperature(64.1f);
        // wait_ms(1000);
        // disp.print_temperature(3.1f);
        // wait_ms(1000);
        // disp.print_temperature(113.3f);
        // wait_ms(1000);
        // disp.print_temperature(-145.8f);
        // wait_ms(1000);
        update_temp_ctrl = false;
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
            debug_if(DEBUG_ON, "update_time - get_time ERROR occured! ERR:%d\r\n", ret);
        }
        else
        {
            debug_if(DEBUG_ON, "Current time: %s\r\n", SystemRtc::time_date_string(current_time));
            blink_on = blink_on^1;
            disp.print_time(current_time.hours, current_time.minutes, blink_on);
        }
        update_time = false;
    }
}

//------------------------------------------------------------------------------