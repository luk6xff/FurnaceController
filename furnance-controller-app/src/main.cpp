#include "mbed.h"
#include "../lib/DS1820/platform/mbed/ds1820-mbed.h"
#include "../lib/TM1637/TM1637.h"



DigitalOut led(LED1);


int main()
{
    // TM1637
    #define LED_DIO PA_0
    #define LED_CLK PA_5
    TM1637::DisplayData_t all_str  = {0xFF, 0xFF, 0xFF, 0xFF};
    TM1637 display(LED_DIO, LED_CLK);      //F401
    display.cls();
    display.writeData(all_str);
    wait_ms(1000);
    display.setBrightness(TM1637_BRT0);
    wait_ms(500);
    display.setBrightness(TM1637_BRT1);
    wait_ms(500);
    display.setBrightness(TM1637_BRT2);
    wait_ms(500);
    display.setBrightness(TM1637_BRT3);
    wait_ms(500);
    display.setBrightness(TM1637_BRT4);
    wait_ms(500);
    display.setBrightness(TM1637_BRT5);
    wait_ms(500);
    display.setBrightness(TM1637_BRT6);
    wait_ms(500);
    display.setBrightness(TM1637_BRT7);
    wait_ms(500);
    display.cls();
    wait_ms(1000);
    display.setBrightness(TM1637_BRT3);
    display.writeData(all_str);
    wait_ms(1000);
    display.cls();
    display.printf(" HI ");
    wait_ms(2000);
    display.printf("GIT!");

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
                display.cls();
                display.printf("%s", buf);
                wait_ms(2000);
            }
        }
    }
}
