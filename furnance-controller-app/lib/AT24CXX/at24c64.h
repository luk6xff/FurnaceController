/**
 *  @brief:   EEPROM AT24C64 library
 *  @author:  luk6xff
 *  @email:   lukasz.uszko@gmail.com
 *  @date:    2019-11-25
 *  @license: MIT
 */

#ifndef __AT24C64_H__
#define __AT24C64_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>



#define AT24CXX_ADDR  0xA0

typedef enum
{
    AT24C64_NOERR = 0,
    AT24C64_BAD_ADDR,       // bad I2C device address
    AT24C64_I2C_ERR,        // I2C error (nack)
    AT24C64_PARAM_ERR,      // Invalid parameter
    AT24C64_OUT_OF_RANGE,   // Data addr out of memory range
} at24cxx_status;


typedef enum
{
    AT24C01 = 0,
    AT24C02,
    AT24C04,
    AT24C08,
    AT24C16,
    AT24C32,
    AT24C64,
    AT24C128,
    AT24C256,
    AT24C512,
    AT24C1024,
    AT24CINVALID,
} at24cxx_type;


typedef struct 
{
    at24cxx_type type;  // Memory type
    uint32_t size;      // Memory size in bytes
    uint32_t page_size; // Memory Page size in bytes    
} at24cxx_mem;




typedef 
{
    at24cxx_type type;   // Memory type
    uint8_t      addr;   // I2C memory address
} at24cxx;

/**
 * @brief Initialize eeprom.
 *
 * @param i2c_addr  I2C chip address.
 * @param mem_type  memory type
 */
at24cxx_status at24c64_init(at24cxx* dev, uint8_t i2c_addr, at24cxx_type mem_type);

/**
 * @brief Write a byte into memory.
 *
 * @param addr       Start address.
 * @param data       Bytes data to be written into memory.
 * @param data_size  Bytes data size.
 */
at24cxx_status at24c64_write(const at24cxx* const dev, uint32_t addr, 
                             uint8_t* data, uint32_t data_size);


/**
 * @brief Write a byte into memory.
 *
 * @param addr  Start address.
 * @param data  A byte to be written into memory.
 */
at24cxx_status at24c64_read(const at24cxx* const dev, uint32_t addr,
                            uint8_t data);

/**
 * @brief Deinitalize eeprom.
 *
 */
void at24c64_deinit(void);

/**
 * @brief Write a byte into memory.
 *
 * @param addr  Start address.
 * @param data  A byte to be written into memory.
 */
at24cxx_status at24c64_write_bytes(uint16_t addr, uint8_t data);

/**
 * @brief Read byte from memory.
 *
 * @param       addr  Start address.
 * @param [IN]  data  A byte to be read into memory.
 * @retval Status 
 */
at24cxx_status at24c64_read_byte(uint16_t addr, uint8_t* data);

/**
 * @brief Write page into memory.
 *
 * @param  addr     Start address.
 * @param  buf      Data to be written.
 * @param  buf_size  Number of bytes to be written (page_size bytes max).
 * @retval Status 
 */
at24cxx_status at24c64_write_page(uint16_t addr, uint8_t* buf, size_t buf_size);


/**
 * @brief Check if write or read operation can succeed.
 *
 * @param  addr     Start address.
 * @param  buf_size  Number of bytes to be written/read.
 * @retval Status value
 */
bool at24c64_check_space(const at24cxx* dev, uint16_t addr, size_t size);


/**
 * @brief Get eeprom size in bytes
 * @param none
 * @return size in bytes (uint32_t)
 */
uint32_t at24cxx_size(at24cxx* dev, void);


/**
 * @brief Clear eeprom (write with 0)
 *
 */
void at24cxx_clear(at24cxx* dev, void);


//-----------------------------------------------------------------------------
// @brief HW dependent functions - must be defined for each platform
//-----------------------------------------------------------------------------
/**
 * @brief Init IO
 *
 * @retval Status value
 */
extern at24cxx_status at24c64_io_init();

/**
 * @brief Deinit IO
 *
 * @retval Status value
 */
extern at24cxx_status at24c64_io_deinit();

/**
 * @brief Write bytes into memory.
 *
 * @param       addr     Start address.
 * @param       buf      Data to be written
 * @param       buf_size  Number of bytes to be written (32 bytes max).
 * @retval Status value
 */
extern at24cxx_status at24c64_write_buffer(uint16_t addr, uint8_t* buf, size_t buf_size);

/**
 * @brief Read bytes from memory.
 *
 * @param       addr     Start address.
 * @param [IN]  buf      Buffer to fill with read bytes.
 * @param       buf_size  Number of bytes to read (32 bytes max).
 * @retval Status value
 */
extern at24cxx_status at24c64_read_buffer(uint16_t addr, uint8_t* buf, size_t buf_size);

/**
 * @brief Enable/Disable Write protection pin.
 *
 * @param enable  enables if true ot disables if false WriteProtectionPin
 */
extern void at24c64_enable_wp(bool enable);

/**
 * @brief Miliseconds delay.
 *
 * @param delay_ms: Delay in miliseconds.
 */
extern void at24c64_delay_ms(uint32_t delay_ms);


#ifdef __cplusplus
}
#endif

#endif /*__AT24C64_H__ */

