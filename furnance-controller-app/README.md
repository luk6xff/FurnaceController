# Furnace Controller platformio-mbed app
Simple pump controller for wood, coal fired furnance.

![alt text](https://raw.githubusercontent.com/luk6xff/FurnanceController/master/docs/photos/p1.jpg)
![alt text](https://raw.githubusercontent.com/luk6xff/FurnanceController/master/docs/photos/p2.jpg)


## Install platformio
```
sudo apt-get remove code --purge
sudo apt-get install code="1.39.2-1571154070"
sudo apt-mark hold code
```

## Parts used
* [STM32 Blue pill](https://os.mbed.com/users/hudakz/code/STM32F103C8T6_Hello/)
* [RTC DS3231 + eeprom AT24C32](http://electropark.pl/rtc-zegary-czasu-rzeczywistego/4409-modul-rtc-wysokiej-precyzji-z-i2c-ds3231.html)
* [Temp sensor DS18B20](http://electropark.pl/czujniki-temperatury/10323-sonda-temperatury-ds18b20-wodoodporna-5m.html)
* [Case Z27](http://electropark.pl/obudowy-do-zasilaczy/4262-obudowa-z27-121x71x45mm-czarna-z27-do-zasilacza.html)
* [7SegDisplay - TM1637](http://electropark.pl/modul-wyswietlaczy/14115-modul-wyswietlacza-7-segmentowego-zolty-50x19mm-kropki.html)
* [Relay](http://electropark.pl/moduly-wejsc-wyjsc/3083-modul-1-przekaznika-5v.html)
* [Tact Switch](http://electropark.pl/tact-switch/81-tact-switch-6x6x14mm-czarny.html)

## FAQ
* Installing platformIO fails:
Run ```sudo apt-get install python3-venv``` and try again.

* Could not open port '/dev/ttyUSB0'
```
sudo usermod -a -G tty $USER
sudo usermod -a -G dialout $USER
sudo usermod -a -G plugdev $USER
logout
login
```

* Udev rules invalid errors eg. `Error: libusb_open() failed with LIBUSB_ERROR_ACCESS`
```
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/master/scripts/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
sudo service udev restart
```
More details here: https://docs.platformio.org/en/latest/faq.html#platformio-udev-rules
