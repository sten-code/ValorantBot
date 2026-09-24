# Mouse

You will have to spoof the PID and VID before you can use it with Valorant.

## Obtaining the PID and VID

First you will have to find out the PID and VID of your mouse. You can do this by going into `Device Manager` and finding your mouse. Right click it and go to `Properties`. Then go to the `Details` tab and select `Hardware Ids` from the dropdown. You will see something like this:
```
USB\VID_413C&PID_301A&REV_0100&MI_00
USB\VID_413C&PID_301A&MI_00
```
The VID is the 4 characters after `VID_` and the PID is the 4 characters after `PID_`. In this case, the VID is `413C` and the PID is `301A`.

## Spoofing the PID and VID

Open `%localappdata%\Arduino15\packages\arduino\hardware\avr\1.8.6\boards.txt` on Windows or `~/.arduino15/packages/arduino/hardware/avr/1.8.6/boards.txt` on Linux.
Find `leonardo.build.pid` and `leonardo.build.vid` and set them to the PID and VID you found earlier.
In this case it should look like this:
```
leonardo.build.vid=0x413c
leonardo.build.pid=0x301a
```
Make sure you restart the Arduino IDE after changing the `boards.txt` file.

## Uploading the firmware

Go into the Arduino IDE and install [HID-Project](https://github.com/NicoHood/HID) by [NicoHood](https://github.com/NicoHood).
Open the `mouse.ino` file in the IDE and upload it to the Arduino.