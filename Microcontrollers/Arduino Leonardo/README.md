# Arduino Leonardo

You will have to spoof the PID and VID before you can use it with Valorant.

- Go into the Arduino IDE and install [HID-Project](https://github.com/NicoHood/HID) by [NicoHood](https://github.com/NicoHood)
- Run `pip install pyusb`
- Run the `list_devices.py` file with python and find the PID and VID of your mouse.
- Open `%localappdata%\Arduino15\packages\arduino\hardware\avr\1.8.6\boards.txt` on Windows or `~/.arduino15/packages/arduino/hardware/avr/1.8.6/boards.txt` on Linux.
- Find `leonardo.build.pid` and `leonardo.build.vid` and set them to the PID and VID of your mouse.
    - Make sure you restart the Arduino IDE after changing the `boards.txt` file.
- Open the `mouse.ino` file in the IDE and upload it to the Arduino.
- Now go into the `main.cpp` file in `ValorantBot/src` and go to this line and replace the vid pid and serial number to the ones you got with the `list_devices.py` file. (the serial number is also given in `list_devices.py`)
```cpp
hid_device* handle = hid_open(0x1038, 0x1858, L"CHIDLC");
```