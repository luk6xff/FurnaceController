#ifndef __TEMP_CONTROLLER_H__
#define __TEMP_CONTROLLER_H__

#include "../lib/DS1820/platform/mbed/ds1820-mbed.h"
#include "app_status.h"

class TempController
{

public:
    typedef struct
    {
        int temp_min;
        int temp_max;
        int temp_relay_on;
        int temp_relay_off;
        int temp_diff_valid; // Difference between current and previous measurement to judge if it's valid.
        int num_of_invalid_measurements_to_err; // Number of invalid measurements to set the error
    } TempCtrlSettings;

    typedef enum
    {
        TEMP_CTRL_RELAY_OFF = 0,
        TEMP_CTRL_RELAY_ON,
    } TempCtrlRelayStatus;


public:
    explicit TempController(const TempCtrlSettings& temp_thresh);
    AppStatus get_temperature(float& temperature);
    float get_last_valid_temperature() const;
    TempCtrlRelayStatus get_relay_status() const;
    void update_temp_thresholds(const TempCtrlSettings& temp_thresh);
    AppStatus process();

private:
    void enable_relay(TempCtrlRelayStatus state);

private:
    ds1820 ds18b20_dev;
    ds1820_mbed ds18b20_mbed_dev;

    TempCtrlSettings temp_thresholds;
    DigitalOut relay_pin;
    TempCtrlRelayStatus relay_status;

    float last_valid_temperature;
    int last_invalid_temperature_counter;

};


#endif // __TEMP_CONTROLLER_H__
