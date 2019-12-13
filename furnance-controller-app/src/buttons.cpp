#include "buttons.h"
#include "hw_config.h"


//------------------------------------------------------------------------------
Buttons::Buttons()
    : btn_ok(BTN_OK)
    , btn_down(BTN_DOWN)
    , btn_up(BTN_UP)
{
    // Register all the buttons
    buttons_map.insert({BtnTypeOk, &btn_ok});
    buttons_map.insert({BtnTypeDown, &btn_down});
    buttons_map.insert({BtnTypeUp, &btn_up});
}

//------------------------------------------------------------------------------
ButtonState Buttons::check_button(ButtonType type)
{
    auto it = buttons_map.find(type);
    if (it == buttons_map.end())
    {
        return BtnStateInvalid;
    }
    return it->second->get_last_state();
}

//------------------------------------------------------------------------------