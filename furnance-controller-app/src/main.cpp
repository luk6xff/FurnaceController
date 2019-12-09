#include "mbed.h"
#include "../lib/AT24CXX/platform/mbed/at24cxx-mbed.h"
#include "../lib/DS3231/platform/mbed/ds3231-mbed.h"
#include "../lib/DS1820/platform/mbed/ds1820-mbed.h"
#include "../lib/TM1637/platform/mbed/tm1637-mbed.h"



DigitalOut led(LED1);


int main()
{

    // AT24CXX
    #define AT24CXX_I2C_ADDR 0x07 // A0=1, A1=1, A2=1
    #define AT24CXX_SDA PB_7
    #define AT24CXX_SCL PB_6
    #define AT24CXX_WP PB_8
    I2C i2c(AT24CXX_SDA, AT24CXX_SCL);
    at24cxx_mbed at24c32_mbed =
    {
        &i2c,
        new DigitalOut(AT24CXX_WP)
    };
    at24cxx at24c32 =
    {
        .type = AT24C32,
        .addr = AT24CXX_I2C_ADDR,
    };
    at24cxx_mbed_init(&at24c32, &at24c32_mbed);
    // EEPROM tests
    #define TEST_DATA_SIZE 2055
    // Write data
    static uint8_t test_data_w[TEST_DATA_SIZE];
    for (int i = 0; i < TEST_DATA_SIZE; i++)
    {
        test_data_w[i] = 0x13;
    }
    if (at24cxx_write(&at24c32, 367, test_data_w, TEST_DATA_SIZE) != AT24CXX_NOERR)
    {
        debug("AT24C32 - WRITE ERROR OCCURED!!!\r\n");
    }
    // Read data
    static uint8_t test_data_r[TEST_DATA_SIZE];
    if (at24cxx_read(&at24c32, 367, test_data_r, TEST_DATA_SIZE))
    {
        debug("AT24C32 - READ ERROR OCCURED!!!\r\n");
    }
    bool is_different = false;
    for (int i = 0; i < TEST_DATA_SIZE; i++)
    {
        if (test_data_r[i] != test_data_w[i])
        {
            debug("AT24C32 - diff values: i=%d,  W:[0x%02x] vs R:[0x%02x]\r\n",i, test_data_w[i], test_data_r[i]);
            is_different = true;
        }
    }
    if (!is_different)
    {
        debug("AT24C32 - R and W data is sthe same!!!\r\n");
    }




    // DS3231
    #define DS3231_I2C_ADDR 0x68
    #define DS3231_SDA PB_7
    #define DS3231_SCL PB_6
    //I2C i2c(DS3231_SDA, DS3231_SCL); @note common i2c with AT24CXX
    ds3231_mbed_init(&i2c, DS3231_I2C_ADDR);




    // TM1637
    #define LED_DIO PA_0
    #define LED_CLK PA_5
    tm1637_mbed disp_mbed =
    {
        DigitalInOut(LED_DIO),
        DigitalOut(LED_CLK)
    };
    tm1637 disp;
    const char str[]  = {"HI.L"};
    const uint8_t raw[]  = {0x31, 0x32, 0x33, 0x34};
    tm1637_mbed_init(&disp, &disp_mbed);
    tm1637_clear(&disp);
    tm1637_print(&disp, (const uint8_t*)str, sizeof(str));
    wait_us(1000*2000);
    tm1637_print(&disp, raw, sizeof(raw));
    wait_us(1000*2000);
    tm1637_print(&disp, (const uint8_t*)str, sizeof(str));
    tm1637_set_brightness(&disp, TM1637_BRT0);
    wait_us(1000*500);
    tm1637_set_brightness(&disp, TM1637_BRT1);
    wait_us(1000*500);
    tm1637_set_brightness(&disp, TM1637_BRT2);
    wait_us(1000*500);
    tm1637_set_brightness(&disp, TM1637_BRT3);
    wait_us(1000*500);
    tm1637_set_brightness(&disp, TM1637_BRT4);
    wait_us(1000*500);
    tm1637_set_brightness(&disp, TM1637_BRT5);
    wait_us(1000*500);
    tm1637_set_brightness(&disp, TM1637_BRT6);
    wait_us(1000*500);
    tm1637_set_brightness(&disp, TM1637_BRT7);
    wait_us(1000*500);
    tm1637_set_brightness(&disp, TM1637_BRT3);
    tm1637_clear(&disp);
    tm1637_print(&disp, (const uint8_t*)str, sizeof(str));
    wait_us(1000*1000);
    tm1637_clear(&disp);




    // DS18B20
    const int MAX_SENSORS = 16;
    #define DS18B20_DATA_PIN PE_12
    int devices_found=0;
    ds1820_mbed_init(DS18B20_DATA_PIN, NC);
    // Initialize global state variables
    ds1820_search_rom_setup();
    // Loop to find all devices on the data line
    while (ds1820_search_rom() && devices_found<MAX_SENSORS-1)
    {
        devices_found++;
    }

    if (devices_found==0)
    {
        debug("No devices found");
    }
    else
    {
        while (true)
        {
            ds1820_convert_temperature(ALL);

            for (int i=0; i<devices_found; i++)
            {
                float temp = ds1820_read_temperature(CELSIUS);
                debug("%3.1f\r\n",temp);
                char buf[4];
                sprintf(buf, "%2.1f*",temp);
                tm1637_clear(&disp);
                tm1637_print(&disp, (const uint8_t*)buf, 4);
                wait_us(1000*2000);
            }
        }
    }
}
