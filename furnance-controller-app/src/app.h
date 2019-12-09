#ifndef __APP_H__
#define __APP_H__

#include "system_rtc.h"
#include "buttons.h"
#include "display.h"
// #include "settings-time.h"
// #include "settings-temp.h"



class App
{

public:
    typedef enum
    {
        MainApp = 0,
        Measurement,
        Settings,
        Error,
    } AppState;

    static App& instance();
    void run();

private:
  App();
  // State methods
  int main_app();
  int settings_menu();

  // Event handlers
  void ev_update_temperature();
  void ev_update_time();

  // Others
  bool update_temp();

private:
    AppState current_state;
    Display disp;
    Buttons btns;
    // Event flags
    volatile bool update_temperature;
    volatile bool update_time;
};

#endif // __APP_H__