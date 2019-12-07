#include "mbed.h"
#include "../lib/DS1820/platform/mbed/ds1820-mbed.h"
#include "../lib/DS3231/platform/mbed/ds3231-mbed.h"
#include "../lib/TM1637/platform/mbed/tm1637-mbed.h"



DigitalOut led(LED1);


int main()
{
    // DS3231
    #define RTC_SDA PB_7
    #define RTC_SCL PB_6
    ds3231_mbed_init(RTC_SDA, RTC_SCL);


    // TM1637
    #define LED_DIO PA_0
    #define LED_CLK PA_5
    tm1637 disp;
    tm1637_display_data all_str  = {0xFF, 0xFF, 0xFF, 0xFF};
    tm1637_mbed_init(&disp, LED_DIO, LED_CLK);
    tm1637_clear();
    tm1637_write_data(all_str, sizeof(all_str), 0);
    wait_ms(1000);
    tm1637_set_brightness(&disp, TM1637_BRT0);
    wait_ms(500);
    tm1637_set_brightness(&disp, TM1637_BRT1);
    wait_ms(500);
    tm1637_set_brightness(&disp, TM1637_BRT2);
    wait_ms(500);
    tm1637_set_brightness(&disp, TM1637_BRT3);
    wait_ms(500);
    tm1637_set_brightness(&disp, TM1637_BRT4);
    wait_ms(500);
    tm1637_set_brightness(&disp, TM1637_BRT5);
    wait_ms(500);
    tm1637_set_brightness(&disp, TM1637_BRT6);
    wait_ms(500);
    tm1637_set_brightness(&disp, TM1637_BRT7);
    wait_ms(500);
    tm1637_clear();
    wait_ms(1000);
    tm1637_set_brightness(&disp, TM1637_BRT3);
    tm1637_write_data(all_str, sizeof(all_str), 0);
    wait_ms(1000);
    tm1637_clear();
    //display.printf(" HI ");
    wait_ms(2000);
    //display.printf("GIT!");

    // DS18B20
    const int MAX_SENSORS = 16;
    #define DS18B20_DATA_PIN PE_12
    int i;
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

            for (i=0; i<devices_found; i++)
            {
                float temp = ds1820_read_temperature(CELSIUS);
                debug("%3.1f\r\n",temp);
                char buf[4];
                sprintf(buf, "%2.1f*",temp);
                tm1637_clear();
                //display.printf("%s", buf);
                wait_ms(2000);
            }
        }
    }
}
