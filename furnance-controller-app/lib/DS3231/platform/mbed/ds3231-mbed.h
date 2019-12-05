
/**
 *  @brief:  Implementation of a DS3231 platform dependent [MBED] functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2019-12-02
 */


#ifndef __DS3231_MBED_H__
#define __DS3231_MBED_H__

#include "mbed.h"
#include "../../ds3231.h"


/** DS3231 Dallas 1-Wire Temperature Probe
 *
 * Example:
 * @code
 *   #include "mbed.h"
 *
 *   @endcode
 */

void ds3231_mbed_init(PinName sda, PinName scl);

void ds3231_mbed_deinit();

#endif // __DS3231_MBED_H__

