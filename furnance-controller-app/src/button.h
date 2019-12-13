#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "mbed.h"


#define SAMPLING_PERIOD_MS  50 // ms

typedef enum
{
    BtnNotPressed   = -2,
    BtnFirstPressed = -1,
    BtnPressed      = 100/SAMPLING_PERIOD_MS,
    BtnHold_1s      = 1000/SAMPLING_PERIOD_MS,
    BtnHold_5s      = 5000/SAMPLING_PERIOD_MS,
    BtnHold_20s     = 20000/SAMPLING_PERIOD_MS,
    BtnHoldSteady,
    BtnStateInvalid = 999999,
} ButtonState;


class Button
{

public:
    explicit Button(PinName pin_name);

    ButtonState get_last_state();

private:
    void raise_isr();
    void fall_isr();
    void sample_isr();

    void reset_state();

private:
    InterruptIn input_button;
    volatile ButtonState current_state;
    volatile ButtonState last_valid_state;
    Ticker ticker;
    volatile uint32_t sampling_counter;
};


#endif // __BUTTON_H__