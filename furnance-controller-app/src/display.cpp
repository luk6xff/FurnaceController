#include "display.h"
#include "hw_config.h"


//------------------------------------------------------------------------------
Display::Display()
    : disp_mbed{DigitalInOut(TM1637_DIO), DigitalOut(TM1637_CLK)}
{
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
void Display::print_temperature(float temp)
{
    const uint8_t num_of_chars = 5;
    char buf[num_of_chars];
    int dec_temp = int(temp);

    if (dec_temp >= 0)
    {
        if (dec_temp < 10)
        {
            sprintf(buf, "%04.1f", temp);
            buf[4] = 1; // Degrees char
        }
        else if (dec_temp < 100)
        {
            sprintf(buf, "%2.1f", temp);
            buf[4] = 1; // Degrees char
        }
        else
        {
            sprintf(buf, "%d", dec_temp);
            buf[3] = 1; // Degrees char
        }
    }
    else
    {
        if (dec_temp < -10)
        {
            sprintf(buf, "%d", dec_temp);

            if (dec_temp > -100)
            {
                buf[3] = 1; // Degrees char
            }
        }
        else
        {
            snprintf(buf, num_of_chars, "%1.1f",temp);
            buf[4] = 1; // Degrees char
        }
    }

    tm1637_clear(&disp);
    tm1637_set_brightness(&disp, TM1637_BRT6);
    tm1637_print(&disp, (const uint8_t*)buf, num_of_chars);
}


//------------------------------------------------------------------------------
void Display::print_time(uint8_t hour, uint8_t min, bool blink)
{
    const uint8_t buf_size = 5;
    uint8_t buf[buf_size];

    buf[0] = hour / 10;
    buf[1] = hour % 10;
    if (blink)
    {
        buf[2] = '.';
        buf[3] = min / 10;
        buf[4] = min % 10;
    }
    else
    {
        buf[2] = min / 10;
        buf[3] = min % 10;
        buf[4] = 0;
    }
    tm1637_clear(&disp);
    tm1637_set_brightness(&disp, TM1637_BRT3);
    tm1637_print(&disp, (const uint8_t*)buf, buf_size);
}

//------------------------------------------------------------------------------
void Display::print_error(AppError error)
{
    tm1637_clear(&disp);
    tm1637_set_brightness(&disp, TM1637_BRT5);
    tm1637_print(&disp, (const uint8_t*)app_errors[error], 4);
}

//------------------------------------------------------------------------------