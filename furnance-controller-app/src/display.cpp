#include "display.h"
#include "hw_config.h"


//------------------------------------------------------------------------------
Display::Display()
{

    tm1637_mbed disp_mbed =
    {
        DigitalInOut(TM1637_DIO),
        DigitalOut(TM1637_CLK)
    };
    tm1637_mbed_init(&disp, &disp_mbed);
}

//------------------------------------------------------------------------------
void Display::print(const char* data, size_t data_len)
{
    tm1637_print(&disp, (const uint8_t*)data, data_len);
}

//------------------------------------------------------------------------------
void Display::clear()
{
    tm1637_clear(&disp);
}

//------------------------------------------------------------------------------
void Display::set_brightness(uint8_t brightness)
{
    tm1637_set_brightness(&disp, brightness);
}


//------------------------------------------------------------------------------
void Display::enable(bool on_off)
{
    tm1637_set_display(&disp, on_off);
}

//------------------------------------------------------------------------------