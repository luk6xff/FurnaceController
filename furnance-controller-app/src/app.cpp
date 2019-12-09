#include "app.h"
#include "mbed.h"


//------------------------------------------------------------------------------
App &App::instance()
{
    static App app;
    return app;
}

//------------------------------------------------------------------------------
App::App()
{
    debug("--- Hello! System here is living! ---");
    //millisStart();
}

//------------------------------------------------------------------------------
void App::run()
{
    while(true)
    {
        switch(current_state)
        {
            case MainApp:
            {
                int res = main_app();
                if (res == 1)
                {
                    debug("Measurement started ...");
                    current_state = Measurement;
                }
                else if (res == 2)
                {
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
int App::main_app()
{
    Ticker update_temp_ticker;
    update_temp_ticker.attach(this, &App::ev_update_temperature, 1); // every 1 second

    Ticker update_time_ticker;
    update_time_ticker.attach(this, &App::ev_update_time, 1); // every 1 second

    while(1)
    {
        Buttons::BtnInfo info = btns.check_buttons();
        if (info.state[Buttons::ButtonType::BtnOk] == Buttons::ButtonState::BtnPressed)
        {
            return 1;
        }
        else if (info.state[Buttons::ButtonType::BtnLeft] == Buttons::ButtonState::BtnPressed ||
                 info.state[Buttons::ButtonType::BtnRight] == Buttons::ButtonState::BtnPressed)
        {
            return 2;
        }

        if (update_temperature)
        {

            update_temperature = false;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
int App::settings_menu()
{

    while(1)
    {
        Buttons::BtnInfo info = btns.check_buttons();
        if (info.state[Buttons::ButtonType::BtnOk] == Buttons::ButtonState::BtnPressed)
        {
            // TODO
            return 1;
        }
        else if (info.state[Buttons::ButtonType::BtnLeft] == Buttons::ButtonState::BtnPressed)
        {
            // TODO
            return 2;
        }
        else if (info.state[Buttons::ButtonType::BtnRight] == Buttons::ButtonState::BtnPressed)
        {
            // TODO
            return 3;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
void App::ev_update_temperature()
{
    update_temperature = true;
}

//------------------------------------------------------------------------------
void App::ev_update_time()
{
    if (current_state == MainApp)
    {
        SystemTime current_time;
        if (SystemRtc::instance().get_time(current_time))
        {
            debug("update_time - get_time ERROR occured !");
        }
        else
        {
            debug("Current time: %s", SystemRtc::time_date_string(current_time));
        }
    }
}

//------------------------------------------------------------------------------
bool App::update_temp()
{
    return true;
}


//------------------------------------------------------------------------------