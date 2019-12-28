# Furnace Controller platformio-mbed app
Simple pump controller SW for wood and coal fired furnance.

[img1]: ./../docs/photos/p1.jpg "Image 1"
[img2]: ./../docs/photos/p2.jpg "Image 2"


## Install platformio
```
sudo apt-get remove code --purge
sudo apt-get install code="1.39.2-1571154070"
sudo apt-mark hold code
```

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
