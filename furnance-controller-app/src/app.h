#ifndef __APP_H__
#define __APP_H__

#include "system_rtc.h"
#include "buttons.h"
#include "display.h"
#include "temp_controller.h"
#include "app_settings.h"
#include "app_errors.h"

#define WATCHDOG_TIMEOUT_MS 10000


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
    int main_app();
    int settings_menu();
    void error_handler();

    // Event handlers
    void ev_update_temp_ctrl();
    void ev_update_time();
    void ev_kick_watchdog();

    // Others
    void check_temp_ctrl();
    void check_time(bool show=false);
    void set_error(AppError err);
    AppError get_error() const;

private:
    AppState current_state;
    AppError current_error;
    Display disp;
    Buttons btns;
    TempController tctrl;

    Watchdog& watchdog;
    Ticker watchdog_ticker;
    // Event flags
    volatile bool update_temp_ctrl;
    volatile bool update_time;
};

#endif // __APP_H__