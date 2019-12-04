/**
 *  @brief:   DS18B20 sensor library.
 *  @author:  luk6xff
 *  @email:   lukasz.uszko@gmail.com
 *  @date:    2019-12-03
 *  @license: MIT
 */


#ifndef __DS1820_H__
#define __DS1820_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef enum 
{
    THIS,   // Command applies to only this device
    ALL     // Command applies to all devices
} DS1820_Devices_t;   

typedef enum 
{
    CELSIUS,     // Celsius degre
    FAHRENHEIT   // Fahrenheit degree
} DS1820_Scale_t;



//------------------------------------------------------------------------------
// @brief FUNCTIONS
//------------------------------------------------------------------------------
/**
 * @brief This function initializes sensor state
 * 
 * @param[in] parasite_powered - if device parsite-powered
 */
void ds1820_init(bool parasite_powered);

/**
 * @brief This function copies the DS1820's RAM into the object's
 *        RAM[].
 */
void ds1820_read_ram(void);

/** 
 * @brief This routine initializes the global variables used in
 *        the ds1820_search_rom() and ds1820_search_alarm() funtions. It should
 *        be called once before looping to find devices.
 */
void ds1820_search_rom_setup(void);

/** 
 * @brief This routine will search for an unidentified device
 *        on the bus. It uses the variables in ds1820_search_rom_setup
 *        to remember the pervious ROM address found.
 *        It will return FALSE if there were no new devices
 *        discovered on the bus.
 */
bool ds1820_search_rom(void);

/** 
 * @brief This routine will search for an unidentified device
 *        which has the temperature alarm bit set. It uses the 
 *        variables in ds1820_search_rom_setup to remember the pervious 
 *        ROM address found. It will return FALSE if there were 
 *        no new devices with alarms discovered on the bus.
 */
bool ds1820_search_alarm(void);

/** 
 * @brief This routine will read the ROM (Family code, serial number
 *        and Checksum) from a dedicated device on the bus.
 *
 * @note This command can only be used when there is only one 
 *       DS1820 on the bus. If this command is used when there 
 *       is more than one slave present on the bus, a data 
 *       collision will occur when all the DS1820s attempt to 
 *       respond at the same time.
 */
void ds1820_read_rom(void);

/** 
 * @brief This routine will initiate the temperature conversion within
 *        a DS1820. There is a built in 750ms delay to allow the 
 *        conversion to complete.
 *
 *        To update all sensors on the bus, use a statement such as this:
 *        sensor[0]->ds1820_convert_temperature(DS1820::ALL);
 *
 * @param allows the fnction to apply to a specific device or
 *        to all devices on the 1-Wire bus.
 */
void ds1820_convert_temperature(DS1820_Devices_t device);

/** 
 * @brief This function will return the sensor temperature. This function
 *        uses the count remainding values to interpolate the temperature
 *        to about 1/150th of a degree. Whereas the sensor is not spec to
 *        that precision. It does seem to give a smooth reading to the
 *        tenth of a degree.
 *
 * @param scale, may be either CELSIUS or FAHRENHEIT
 * @returns temperature for that scale
 */
float ds1820_read_temperature(DS1820_Scale_t scale);

/** 
 * @brief This function calculates the ROM checksum and compares it to the
 *        CRC value stored in ROM[7].
 *
 * @returns true if the checksum matches, otherwise false.
 */
bool ds1820_rom_checksum_error(void);

/** 
 * @brief This function calculates the RAM checksum and compares it to the
 *        CRC value stored in RAM[8].
 *
 * @returns true if the checksum matches, otherwise false.
 */
bool ds1820_ram_checksum_error(void);

/** 
 * @brief This function sets the temperature resolution for the DS18B20
 *        in the configuration register.
 *
 * @param a number between 9 and 12 to specify the resolution
 * @returns true if successful
 */ 
bool ds1820_set_configuration_bits(unsigned int resolution);

/** 
 * @brief This function returns the values stored in the temperature
 *        alarm registers. 
 *
 * @returns a 16 bit integer of TH (upper byte) and TL (lower byte).
 */
int ds1820_read_scratchpad(void);

/** 
 * @brief This function will store the passed data into the DS1820's RAM.
 * @note  It does NOT save the data to the EEPROM for retention
 *        during cycling the power off and on.
 *
 * @param a 16 bit integer of TH (upper byte) and TL (lower byte).
 */ 
void ds1820_write_scratchpad(int data);

/** 
 * @brief This function will transfer the TH and TL registers from the
 *        DS1820's RAM into the EEPROM.
 * @note  There is a built in 10ms delay to allow for the
 *        completion of the EEPROM write cycle.
 *
 * @param allows the fnction to apply to a specific device or
 *        to all devices on the 1-Wire bus.
 */ 
void ds1820_store_scratchpad(DS1820_Devices_t device);

/** 
 * @brief This function will copy the stored values from the EEPROM
 *        into the DS1820's RAM locations for TH and TL.
 *
 * @param allows the function to apply to a specific device or
 *        to all devices on the 1-Wire bus.
 */
int ds1820_recall_scratchpad(DS1820_Devices_t device);

/** 
 * @brief This function will return the type of power supply for
 *        a specific device. It can also be used to query all devices
 *        looking for any device that is parasite powered.
 *
 * @returns true if the device (or all devices) are Vcc powered,
 *          returns false if the device (or ANY device) is parasite powered.
 */
bool ds1820_read_power_supply(DS1820_Devices_t device);


//-----------------------------------------------------------------------------
// @brief HW dependent functions - must be defined for each platform
//-----------------------------------------------------------------------------

#ifdef __cplusplus
}
#endif


#endif // __DS1820_H__
