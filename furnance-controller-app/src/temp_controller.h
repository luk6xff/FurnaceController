#ifndef __TEMP_CONTROLLER_H__
#define __TEMP_CONTROLLER_H__

#include "../lib/DS1820/platform/mbed/ds1820-mbed.h"

class TempController
{

public:
    typedef struct
    {
        int temp_min;
        int temp_max;
        int temp_relay_on;
        int temp_relay_off;
    } TempCtrlSettings;


    typedef enum
    {
        TEMP_CTRL_NOERR = 0,
        TEMP_CTRL_TEMP_TOO_LOW,
        TEMP_CTRL_TEMP_TOO_HIGH,
        TEMP_CTRL_NO_SENSOR,
    } TempCtrlError;

    typedef enum
    {
        TEMP_CTRL_RELAY_OFF = 0,
        TEMP_CTRL_RELAY_ON,
    } TempCtrlRelayStatus;


public:
    explicit TempController(const TempCtrlSettings& temp_thresh);
    TempCtrlError get_temperature(float& temperature);
    float get_last_temperature() const;
    TempCtrlRelayStatus get_relay_status() const;
    void update_temp_thresholds(const TempCtrlSettings& temp_thresh);
    TempCtrlError process();

private:
    bool is_sensor_available();
    void enable_relay(TempCtrlRelayStatus state);

private:
    //ds1820 temp_sensor;
    int ds1820_sensors_found;
    const int ds1820_sensors_num;
    TempCtrlSettings temp_thresholds;

    DigitalOut relay_pin;
    TempCtrlRelayStatus relay_status;
    float last_temperature;

};


#endif // __TEMP_CONTROLLER_H__
