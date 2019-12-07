
/**
 *  @brief:  Implementation of a TM1637 platform dependent [MBED] functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2019-12-02
 */


#ifndef __TM1637_MBED_H__
#define __TM1637_MBED_H__

#include "mbed.h"
#include "../../tm1637.h"


/** TM1637 7-segment led driver
 *
 * Example:
 * @code
 *   #include "mbed.h"
 *
 *   @endcode
 */


/**
 * @brief Init function driving TM1637 LED controller.
 *
 * @param  PinName dio Serial bus DIO pin
 * @param  PinName clk Serial bus CLK pin
 */;
void tm1637_mbed_init(tm1637* const dev, PinName dio, PinName clk);

void tm1637_mbed_deinit();

#endif // __TM1637_MBED_H__

