#include "temp_controller.h"
#include "hw_config.h"

#define DEBUG_ON 0

//------------------------------------------------------------------------------
TempController::TempController(const TempCtrlSettings& temp_thresh)
    : temp_thresholds(temp_thresh)
    , relay_pin(RELAY_PIN)
    , relay_status(TEMP_CTRL_RELAY_OFF)
    , last_valid_temperature(-1)
    , last_invalid_temperature_counter(0)
{
    // DS18B20
    ds18b20_mbed_dev.data_pin = new DigitalInOut(DS18B20_DATA_PIN);
    ds18b20_mbed_dev.parasite_pin = nullptr;
    ds18b20_dev.sensors_num = DS18B20_SENSORS_NUM;
    ds1820_mbed_init(&ds18b20_dev, &ds18b20_mbed_dev);

    // RELAY_OFF- always start from disabled state
    relay_pin = 1;
}

//------------------------------------------------------------------------------
AppStatus TempController::get_temperature(float& temperature)
{
    AppStatus err = STATUS_NO_ERROR;
    float t = 0.0f;

    // Is sensor available
    if (!ds1820_read_power_supply(&ds18b20_dev, THIS))
    {
        last_invalid_temperature_counter++;
        err = STATUS_NO_TEMP_SENSOR;
    }
    else
    {
        // Measure temperature
        ds1820_convert_temperature(&ds18b20_dev, THIS);
        t = ds1820_read_temperature(&ds18b20_dev, CELSIUS);
        temperature = t;

        if (t <= temp_thresholds.temp_min)
        {
            last_invalid_temperature_counter++;
            err = STATUS_TOO_LOW_TEMP;
        }
        else if (t >= temp_thresholds.temp_max)
        {
            last_invalid_temperature_counter++;
            err = STATUS_TOO_HIGH_TEMP;
        }
        else if (((int)last_valid_temperature != -1) && \
                 ((t > (last_valid_temperature+temp_thresholds.temp_diff_valid)) || \
                 (t < (last_valid_temperature-temp_thresholds.temp_diff_valid))))
        {
            last_invalid_temperature_counter++;
            err = STATUS_INVALID_TEMP;
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

    return STATUS_NO_ERROR;
}


//------------------------------------------------------------------------------
float TempController::get_last_valid_temperature() const
{
    return last_valid_temperature;
}

//------------------------------------------------------------------------------
AppStatus TempController::process()
{
    float read_temperature;
    AppStatus status = get_temperature(read_temperature);
    if (status != STATUS_NO_ERROR)
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