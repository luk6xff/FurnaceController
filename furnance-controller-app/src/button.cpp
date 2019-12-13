#include "buttons.h"
#include "hw_config.h"


//------------------------------------------------------------------------------
Button::Button(PinName pin_name)
    : input_button(pin_name)
    , last_state(BtnNotPressed)
    , sampling_counter(0)
{
    input_button.mode(PullUp);
    input_button.rise(mbed::callback(this, &Button::raise_isr));
    input_button.fall(mbed::callback(this, &Button::fall_isr));

    ticker.attach_us(mbed::callback(this, &Button::sample_isr), SAMPLING_PERIOD_MS*1000);
}

//------------------------------------------------------------------------------
ButtonState Button::get_state() const
{
    return state;
}

//------------------------------------------------------------------------------
void Button::raise_isr()
{
    if (last_state == BtnNotPressed)
    {
        last_state = BtnFirstPressed;
        sampling_counter = 0;
    }
}

//------------------------------------------------------------------------------
void Button::fall_isr()
{
    if (last_state != BtnFirstPressed)
    {
        last_state = BtnNotPressed;
    }
}

//------------------------------------------------------------------------------
void Button::sample_isr()
{
    switch (last_state)
    {
        case BtnFirstPressed:
        {
            if (sampling_counter++ >= BtnHold_50ms)
            {
                last_state = BtnHold_50ms;
            }
            break;
        }

        case BtnHold_50ms:
        {
            if (input_button)
            {
                if (sampling_counter++ >= BtnHold_1s)
                {
                    last_state = BtnHold_1s;
                }
            }
            else
            {
                last_state = BtnNotPressed;
            }
            break;
        }

        case BtnHold_1s:
        {
            if (input_button)
            {
                if (sampling_counter++ >= BtnHold_5s)
                {
                    last_state = BtnHold_5s;
                }
            }
            else
            {
                last_state = BtnNotPressed;
            }
            break;
        }
        default:
            break;
    }
}

//------------------------------------------------------------------------------
