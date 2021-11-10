#ifndef __APP_H__
#define __APP_H__

#include "system_rtc.h"
#include "buttons.h"
#include "display.h"
#include "temp_controller.h"
#include "app_settings.h"
#include "app_status.h"


class App
{

public:
    typedef enum
    {
        Init = 0,
        MainApp,
        Settings,
        Error,
    } AppState;

    static App& instance();
    void run();

private:
    App();
    // State methods
    void init();
    AppState main_app();
    void settings_menu();
    void error_handler();

    // Event handlers
    void ev_update_param();
    void ev_kick_watchdog();

    // Others
    void check_temp_ctrl(bool show_temp=true);
    void check_time(bool show_time=true);
    void check_indoor_temperature();
    void set_status(AppStatus err);
    AppStatus get_status() const;

private:
    AppState current_state;
    AppStatus current_status;
    AppSettings settings;
    Display disp;
    Buttons btns;
    TempController tctrl;

    Watchdog& watchdog;
    Ticker watchdog_ticker;
    Ticker update_param_ticker;
    Timer app_timer;
    // Event flags
    volatile bool update_temp_ctrl;
    volatile bool update_time;
};

#endif // __APP_H__