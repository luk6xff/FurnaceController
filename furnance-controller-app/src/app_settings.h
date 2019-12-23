#ifndef __APP_SETTINGS_H__
#define __APP_SETTINGS_H__

#include "../lib/AT24CXX/platform/mbed/at24cxx-mbed.h"

// Include all the modules containing non volatile settings
#include "temp_controller.h"
#include "system_rtc.h"
#include "display.h"
#include "buttons.h"

static const uint32_t settings_timeout_s = 15; //[s]

class AppSettings
{

public:

typedef struct
{
    uint32_t magic;
    uint32_t version;
    TempController::TempCtrlSettings temp;

} Settings;

public:
    static AppSettings& instance();
    void init();

    const Settings& get_defaults();
    const Settings& get_current();

    // Settings options
    bool set_date_time(Buttons& btns, Display& disp);
    bool set_temperatures(Buttons& btns, Display& disp);

private:
    bool save_settings(const Settings &settings);
    bool read_rettings();

    void print_current_settings();

private:
    AppSettings();
    Settings default_settings;
    Settings current_settings;

    static const int k_temperature_min = -55;
    static const int k_temperature_max = 125;

    // AT24C32
    I2C at24c32_i2c;
    DigitalOut at24c32_wp;
    at24cxx_mbed at24c32_mbed;
    at24cxx at24c32;
};

#endif // __APP_SETTINGS_H__
