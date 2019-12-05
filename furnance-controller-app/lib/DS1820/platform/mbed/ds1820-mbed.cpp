
/**
 *  @brief:  Implementation of a DS1820 platform dependent [MBED] functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2019-11-15
 */

#include "ds1820-mbed.h"

/**
 * DS1820 pins
 */
DigitalInOut* _data_pin;
DigitalOut* _parasite_pin;

//------------------------------------------------------------------------------
void ds1820_mbed_init(PinName data_pin, PinName parasite_pin)
{
    _data_pin = new DigitalInOut(data_pin);
    _parasite_pin = new DigitalOut(parasite_pin);
    if (parasite_pin == NC)
    {
        ds1820_init(false);
        return;
    }
    ds1820_init(true);
}

//------------------------------------------------------------------------------
void ds1820_mbed_deinit(void)
{
    // IO
    delete(_data_pin);
    delete(_parasite_pin);
}

//------------------------------------------------------------------------------
void ds1820_delay_us(uint32_t us)
{
    wait_us(us);
}

//------------------------------------------------------------------------------
bool ds1820_onewire_reset(void)
{
    // This will return false if no devices are present on the data bus
    bool presence = false;
    _data_pin->output();
    *_data_pin = 0;           // bring low for 500 us
    ds1820_delay_us(500);
    _data_pin->input();       // let the data line float high
    ds1820_delay_us(90);            // wait 90us
    if (_data_pin->read() == 0) // see if any devices are pulling the data line low
    {
        presence = true;
    }
    ds1820_delay_us(410);
    return presence;
}

//------------------------------------------------------------------------------
void ds1820_onewire_bit_out(bool bit_data)
{
    _data_pin->output();
    *_data_pin = 0;
    ds1820_delay_us(5);
    if (bit_data)
    {
        _data_pin->input();    // bring data line high
        ds1820_delay_us(55);
    }
    else
    {
        ds1820_delay_us(55); // keep data line low
        _data_pin->input();
    }
}

//------------------------------------------------------------------------------
bool ds1820_onewire_bit_in(void)
{
    bool answer;
    _data_pin->output();
    *_data_pin = 0;
    ds1820_delay_us(5);
    _data_pin->input();
    ds1820_delay_us(5);
    answer = _data_pin->read();
    ds1820_delay_us(50);
    return answer;
}

//------------------------------------------------------------------------------
void ds1820_set_parasite_pin(bool enable)
{
    if (enable)
    {
        *_parasite_pin = 1;
        return;
    }
    *_parasite_pin = 0;
}

//-----------------------------------------------------------------------------

