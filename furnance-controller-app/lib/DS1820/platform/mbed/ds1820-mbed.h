
/**
 *  @brief:  Implementation of a DS1820 platform dependent [MBED] functions
 *  @author: luk6xff
 *  @email:  lukasz.uszko@gmail.com
 *  @date:   2019-12-02
 */


#ifndef __DS1820_MBED_H__
#define __DS1820_MBED_H__

#include "mbed.h"
#include "../../ds1820.h"


/** DS1820 Dallas 1-Wire Temperature Probe
 *
 * Example:
 * @code
 *   #include "mbed.h"
 *
 *   #define DS18B20_DATA_PIN PE_12
 *   const int MAX_SENSORS = 16;
 *   int main()
 *   {
 *       int i;
 *       int devices_found=0;
 *       ds1820_mbed_init(DS18B20_DATA_PIN, NC);
 *
 *       // Initialize global state variables
 *       ds1820_search_rom_setup();
 *
 *       // Loop to find all devices on the data line
 *       while (ds1820_search_ROM() && devices_found<MAX_SENSORS-1)
 *       {
 *          devices_found++;
 *       }
 *
 *       if (devices_found==0)
 *       {
 *          debug("No devices found");
 *       }
 *       else
 *       {
 *           while (true)
 *           {
 *               ds1820_convert_temperature(ALL);
*
 *               for (i=0; i<devices_found; i++)
 *               {
 *                   float temp = ds1820_read_temperature(CELSIUS);
 *                   debug("%3.1f\r\n",temp);
 *                   wait_ms(2000);
 *               }
 *           }
 *       }
 *   }
 *   @endcode
 */

void ds1820_mbed_init(PinName data_pin, PinName parasite_pin);

void ds1820_mbed_deinit();

#endif // __DS1820_MBED_H__

