#include "temp_controller.h"
#include "hw_config.h"

#define DEBUG_ON 0

//------------------------------------------------------------------------------
TempController::TempController(const TempCtrlSettings& temp_thresh)
    : ds1820_sensors_num(DS18B20_SENSORS_NUM)
    , temp_thresholds(temp_thresh)
    , relay_pin(RELAY_PIN)
    , relay_status(TEMP_CTRL_RELAY_OFF)
    , last_temperature(0.0f)
{
    // DS18B20
    ds1820_mbed_init(DS18B20_DATA_PIN, NC);
    // Initialize global state variables
    ds1820_search_rom_setup();
    // Search for new device
    while (ds1820_search_rom() && ds1820_sensors_found < ds1820_sensors_num)
    {
        ds1820_sensors_found++;
    }

    // RELAY_OFF- always start from disabled state
    relay_pin = 1;
}

//------------------------------------------------------------------------------
TempController::TempCtrlError TempController::get_temperature(float& temperature)
{
    if (!is_sensor_available())
    {
        return TEMP_CTRL_NO_SENSOR;
    }

    ds1820_convert_temperature(ALL);
    temperature = ds1820_read_temperature(CELSIUS);

    if (temperature < temp_thresholds.temp_min)
    {
        return TEMP_CTRL_TEMP_TOO_LOW;
    }

    else if (temperature > temp_thresholds.temp_max)
    {
        return TEMP_CTRL_TEMP_TOO_HIGH;
    }

    return TEMP_CTRL_NOERR;
}

//------------------------------------------------------------------------------
float TempController::get_last_temperature() const
{
    return last_temperature;
}

//------------------------------------------------------------------------------
TempController::TempCtrlError TempController::process()
{
    TempCtrlError status = get_temperature(last_temperature);
    if (status != TEMP_CTRL_NOERR)
    {
        debug_if(DEBUG_ON, "TEMP_CONTROLLER: get_temperature failed, STATUS: %d\r\n", status);
        return status;
    }
    // Enable relay
    if (last_temperature > temp_thresholds.temp_relay_on)
    {
        enable_relay(TEMP_CTRL_RELAY_ON);
    }
    else if (last_temperature <= (temp_thresholds.temp_relay_off))
    {
        enable_relay(TEMP_CTRL_RELAY_OFF);
    }
    return status;
}

//------------------------------------------------------------------------------
TempController::TempCtrlRelayStatus TempController::get_relay_status() const
{
    return relay_status;
}

//------------------------------------------------------------------------------
void TempController::update_temp_thresholds(const TempCtrlSettings& temp_thresh)
{
    temp_thresholds = temp_thresh;
}

//------------------------------------------------------------------------------
bool TempController::is_sensor_available()
{
    return ds1820_read_power_supply(THIS);
}

//------------------------------------------------------------------------------
void TempController::enable_relay(TempCtrlRelayStatus state)
{
    if (relay_status == state)
    {
        return;
    }

    if (state == TEMP_CTRL_RELAY_OFF)
    {
        debug_if(DEBUG_ON, "TEMP_CONTROLLER: RELAY OFF\r\n");
        relay_pin = 1;
    }
    else // TEMP_CTRL_RELAY_ON
    {
        debug_if(DEBUG_ON, "TEMP_CONTROLLER: RELAY ON\r\n");
        relay_pin = 0;
    }

    relay_status = state;
}

//------------------------------------------------------------------------------