#ifndef __APP_SETTINGS_H__
#define __APP_SETTINGS_H__

#include "../lib/AT24CXX/platform/mbed/at24cxx-mbed.h"

// Include all the modules containing non volatile settings
#include "temp_controller.h"


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

    bool save_settings(const Settings &settings);
    bool read_rettings();
    void set_defaults();

private:
    AppSettings();
    Settings default_settings;
    Settings current_settings;

    // AT24C32
    I2C at24c32_i2c;
    DigitalOut at24c32_wp;
    at24cxx_mbed at24c32_mbed;
    at24cxx at24c32;
};

#endif // __APP_SETTINGS_H__