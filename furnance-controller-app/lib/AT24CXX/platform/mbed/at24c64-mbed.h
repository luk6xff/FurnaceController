/**
 *  @brief:   EEPROM AT24C64 library - mbed platform
 *  @author:  luk6xff
 *  @email:   lukasz.uszko@gmail.com
 *  @date:    2019-11-25
 *  @license: MIT
 */

#ifndef __AT24C64_MBED_H__
#define __AT24C64_MBED_H__

#include "mbed.h"
#include "../../at24c64.h"


/**
 * @brief Initialize eeprom with MBED dependent parameters.
 *
 */
void at24c64_mbed_init(PinName sda, PinName scl, PinName wp,
                       uint8_t chip_addr, size_t chip_size, size_t page_size);

/**
 * @brief Deinitalize eeprom.
 *
 */
void at24c64_mbed_deinit(void);


#endif /*__AT24C64_MBED_H__ */

