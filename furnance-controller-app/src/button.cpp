#include "buttons.h"
#include "hw_config.h"

#define DEBUG_ON 0

//------------------------------------------------------------------------------
Button::Button(PinName pin_name)
    : input_button(pin_name)
    , current_state(BtnNotPressed)
    , last_valid_state(BtnNotPressed)
    , sampling_counter(0)
{
    input_button.mode(PullUp);
    input_button.rise(mbed::callback(this, &Button::raise_isr));
    input_button.fall(mbed::callback(this, &Button::fall_isr));

    ticker.attach_us(mbed::callback(this, &Button::sample_isr), SAMPLING_PERIOD_MS*1000);
}

//------------------------------------------------------------------------------
ButtonState Button::get_last_state()
{
    ButtonState tmp = last_valid_state;
    last_valid_state = current_state;
    return tmp;
}

//------------------------------------------------------------------------------
void Button::raise_isr()
{
    if (current_state != BtnFirstPressed)
    {
        reset_state();
        debug_if(DEBUG_ON, "BUTTON: STATE == raise_isr \r\n");
    }
}

//------------------------------------------------------------------------------
void Button::fall_isr()
{
    // First press, button to GND
    if (current_state == BtnNotPressed)
    {
        reset_state();
        current_state = BtnFirstPressed;
        debug_if(DEBUG_ON, "BUTTON: STATE == fall_isr \r\n");
    }
}

//------------------------------------------------------------------------------
void Button::sample_isr()
{
    switch (current_state)
    {
        case BtnFirstPressed:
        {
            sampling_counter++;
            if (sampling_counter == BtnPressed)
            {
                // Is button still hold ?
                if (input_button == 0)
                {
                    debug_if(DEBUG_ON, "BUTTON: STATE CHANGE: last_valid_state = BtnPressed>>>\r\n");
                    last_valid_state = BtnPressed;
                    current_state = BtnHoldSteady;
                }
                else
                {
                    debug_if(DEBUG_ON, "BUTTON: <<<RESETTED !!!>>>\r\n");
                    reset_state();
                }
            }
            break;
        }

        case BtnHoldSteady:
        {
            sampling_counter++;
            // Is button still hold ?
            if (input_button == 0)
            {
                if (sampling_counter == BtnHold_1s)
                {
                    last_valid_state = BtnHold_1s;
                    debug_if(DEBUG_ON, "BUTTON: STATE CHANGE: last_valid_state = BtnHold_1s>>>\r\n");
                }
                else if (sampling_counter == BtnHold_5s)
                {
                    last_valid_state = BtnHold_5s;
                    debug_if(DEBUG_ON, "BUTTON: STATE CHANGE: last_valid_state = BtnHold_5s>>>\r\n");
                }
                else if (sampling_counter == BtnHold_20s)
                {
                    last_valid_state = BtnHold_20s;
                    debug_if(DEBUG_ON, "BUTTON: STATE CHANGE: last_valid_state = BtnHold_20s>>>\r\n");
                }
            }
            else
            {
                reset_state();
            }
            break;
        }

        default:
            break;
    }
}

//------------------------------------------------------------------------------
void Button::reset_state()
{
    debug_if(DEBUG_ON, "BUTTON: <<<RESET>>>>>>>>>>> !!!>>>\r\n");
    current_state = BtnNotPressed;
    last_valid_state = BtnNotPressed;
    sampling_counter = 0;
}

//------------------------------------------------------------------------------