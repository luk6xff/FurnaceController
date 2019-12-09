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

    const Settings& get_defaults();

private:

    AppSettings();
    Settings default_settings;
};

#endif // __APP_SETTINGS_H__