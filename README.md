# FurnaceController
Simple pump controller project for wood and coal fired furnance.

![img1]
(docs/photos/p1.jpg)
![img2]
(docs/photos/p2.jpg)


## Parts used
* [STM32 Blue pill](https://os.mbed.com/users/hudakz/code/STM32F103C8T6_Hello/)
* [RTC DS3231 + eeprom AT24C32](http://electropark.pl/rtc-zegary-czasu-rzeczywistego/4409-modul-rtc-wysokiej-precyzji-z-i2c-ds3231.html)
* [Temp sensor DS18B20](http://electropark.pl/czujniki-temperatury/10323-sonda-temperatury-ds18b20-wodoodporna-5m.html)
* [Case Z27](http://electropark.pl/obudowy-do-zasilaczy/4262-obudowa-z27-121x71x45mm-czarna-z27-do-zasilacza.html)
* [7SegDisplay - TM1637](http://electropark.pl/modul-wyswietlaczy/14115-modul-wyswietlacza-7-segmentowego-zolty-50x19mm-kropki.html)
* [Relay](http://electropark.pl/moduly-wejsc-wyjsc/3083-modul-1-przekaznika-5v.html)
* [Tact Switch](http://electropark.pl/tact-switch/81-tact-switch-6x6x14mm-czarny.html)

## Hardware
Hardware connection pinout to be found at [hw_config.h](furnance-controller-app/src/hw_config.h)

HW device schematic:
![schematic]
(docs/photos/furnance_controller_bb.png)

## Software
Software part related README to be found [here](furnance-controller-app/README.md)