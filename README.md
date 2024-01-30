# Valorant Bot

This is my attempt at making a Valorant Color Bot.

A color bot is a hack that only uses data from the screen in order to do things such as aimbot and triggerbot.

## How it works

This bot uses the Win32 API in order to read pixel values from the screen into a buffer. It converts this buffer of pixels into an OpenCV image, where it uses masking and some filters in order to find out where the enemies are.

It will then find the enemy closest to the crosshair and the highest point in order to estimate where the nearest head of the enemy is.

When the ALT key is pressed it will send the required mouse movements to a microcontroller through the serial interface which will then send back a HID mouse input.

Valorant doesn't allow for multiple mice to work at the same time so it uses a USB Host module to merge the mouse input from your mouse and the aimbot mouse movements so that everything goes through 1 device.

There are multiple options for microcontrollers and different configurations:

## Compiling from Source

You will need a program to send the mouse positions to the Arduino. For this you will have to compile the `ValorantBot` program.


- First run this command or download this repository as a `.zip` file.
    ```bash
    git clone https://github.com/sten-code/ValorantBot
    ```
- Open the `Libs` folder and follow the instructions from the [README.md](https://github.com/sten-code/ValorantBot/tree/master/Libs) in there.
- Open the `ValorantBot.sln` file in Visual Studio.
    - Make sure you have the `Desktop Environment with C++` module installed inside Visual Studio.
- Set the configuration to `Release`, target platform to `x64` and compile by going into the Build menu at the top and by pressing `Build Solution`
- You now have a `.exe` file inside `x64/Release` (there is also a folder called `ValorantBot/x64/Release`, but just ignore this one).

## [Arduino Leonardo](https://store.arduino.cc/products/arduino-leonardo-with-headers)

The way this works is by setting the VID and PID of the Arduino Leonardo to the same VID and PID of your mouse so that Valorant won't disable your Arduino Leonardo.

You can find instructions [here](https://github.com/sten-code/ValorantBot/tree/master/Microcontrollers/Arduino%20Leonardo).

## [Arduino Leonardo](https://store.arduino.cc/products/arduino-leonardo-with-headers) and a [USB Host Shield](https://github.com/felis/USB_Host_Shield_2.0)

This is by far the most popular option as it only requires 2 components, though this usually isn't cheap. If you buy these on standard online stores then it can cost you up to 30-40 USD for both the Arduino Leonardo and the USB Host Shield, but you can get these components for very little on online Chinese stores such as AliExpress. I got an Arduino Leonardo for 1.5 USD + 2 USD shipping. If you don't want to spend much and don't really care about having to wait ~10 days then getting it on these sites is a really good alternative.

- Place the USB Host Shield on top of the Arduino Leonardo.
- You will have to unplug all mice from your PC and plug your main one into the USB Host Shield.
    - Vanguard doesn't allow multiple mice to work at the same time. This is the entire reason you need a USB Host Shield in the first place.

## USB Host Shield no power fix

The USB Host Shield sometimes doesn't come presoldered, there are some pads that must be soldered in order for the power to go from the Arduino to the USB port. This isn't really hard, but it is very annoying because it should just come presoldered by default.

[This](https://www.youtube.com/watch?v=1dxwU87GQVM&t=40s) is a very easy to understand and simple video on how to do this.
