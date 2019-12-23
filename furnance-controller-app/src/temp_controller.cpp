#include "temp_controller.h"
#include "hw_config.h"

#define DEBUG_ON 0

//------------------------------------------------------------------------------
TempController::TempController(const TempCtrlSettings& temp_thresh)
    : ds1820_sensors_num(DS18B20_SENSORS_NUM)
    , temp_thresholds(temp_thresh)
    , relay_pin(RELAY_PIN)
    , relay_status(TEMP_CTRL_RELAY_OFF)
    , last_valid_temperature(-1)
    , last_invalid_temperature_counter(0)
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
    return temperature_sensor_reader(temperature);
}


//------------------------------------------------------------------------------
float TempController::get_last_valid_temperature() const
{
    return last_valid_temperature;
}

//------------------------------------------------------------------------------
TempController::TempCtrlError TempController::process()
{
    float read_temperature;
    TempCtrlError status = get_temperature(read_temperature);
    if (status != TEMP_CTRL_NOERR)
    {
        debug_if(DEBUG_ON, "TEMP_CONTROLLER: get_temperature failed, STATUS: %d\r\n", status);
        return status;
    }


    // Update relay status
    if (last_valid_temperature >= temp_thresholds.temp_relay_on)
    {
        enable_relay(TEMP_CTRL_RELAY_ON);
    }
    else if (last_valid_temperature <= (temp_thresholds.temp_relay_off))
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
TempController::TempCtrlError TempController::temperature_sensor_reader(float& last_temperature)
{
    TempCtrlError err = TEMP_CTRL_NOERR;
    float temperature = 0.0f;

    if (!is_sensor_available())
    {
        last_invalid_temperature_counter++;
        err = TEMP_CTRL_NO_SENSOR;
    }
    else
    {
        // Measure temperature
        ds1820_convert_temperature(THIS);
        temperature = ds1820_read_temperature(CELSIUS);
        last_temperature = temperature;

        if (temperature <= temp_thresholds.temp_min)
        {
            last_invalid_temperature_counter++;
            err = TEMP_CTRL_TEMP_TOO_LOW;
        }
        else if (temperature >= temp_thresholds.temp_max)
        {
            last_invalid_temperature_counter++;
            err = TEMP_CTRL_TEMP_TOO_HIGH;
        }
        else if (((int)last_valid_temperature != -1) && \
                 temperature > (last_valid_temperature+temp_thresholds.temp_diff_valid) || \
                 temperature < (last_valid_temperature-temp_thresholds.temp_diff_valid))
        {
            last_invalid_temperature_counter++;
            err = TEMP_CTRL_TEMP_INVALID;
        }
        else
        {
            last_invalid_temperature_counter = 0;
            last_valid_temperature = temperature;
        }
    }

    if (last_invalid_temperature_counter >= temp_thresholds.num_of_invalid_measurements_to_err)
    {
        return err;
    }

    return TEMP_CTRL_NOERR;
}

//------------------------------------------------------------------------------