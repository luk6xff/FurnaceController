#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "mbed.h"


#define SAMPLING_PERIOD_MS  10 // ms

typedef enum
{
    BtnNotPressed   = 0,
    BtnFirstPressed = 1,
    BtnHold_50ms    = 50/SAMPLING_PERIOD_MS,
    BtnPressed      = BtnHold_50ms,
    BtnHold_1s      = 1000/SAMPLING_PERIOD_MS,
    BtnHold_5s      = 5000/SAMPLING_PERIOD_MS,
    BtnStateInvalid = 0xFFFFFFFF,
} ButtonState;


class Button
{

public:
    explicit Button(PinName pin_name);

    ButtonState get_state() const;

private:
    void raise_isr();
    void fall_isr();
    void sample_isr();

private:
    InterruptIn input_button;
    ButtonState last_state;
    Ticker ticker;
    uint32_t sampling_counter;
};


#endif // __BUTTON_H__