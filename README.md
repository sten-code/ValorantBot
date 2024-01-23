# Valorant Bot

This is my attempt at making a Valorant Color Bot.

A color bot is a hack that only uses data from the screen in order to do things such as aimbot and triggerbot.

## How it works

This bot uses the Win32 API in order to read pixel values from the screen into a buffer. It converts this buffer of pixels into an OpenCV image, where it uses masking and some filters in order to find out where the enemies are.

It will then find the enemy closest to the crosshair and the highest point in order to estimate where the head of the enemy is.

When the ALT key is pressed it will send the required mouse movements to a microcontroller through the serial interface which will then send back a HID mouse input.

Valorant doesn't allow for multiple mice to work at the same time so it uses a USB Host module to merge the mouse input from your mouse and the aimbot mouse movements so that everything goes through 1 device.

There are multiple options for microcontrollers and different configurations:

## [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/), [Arduino Uno](https://store.arduino.cc/products/arduino-uno-rev3) and a [USB Host Shield](https://github.com/felis/USB_Host_Shield_2.0)

I haven't seen anyone use this configuration. The only reason I use this is because I had a Pico laying around.
You can get a Pico for ~4 USD, which makes this a very good option if you already have an Arduino Uno but not an Arduino Leonardo.

- Place the USB Host Shield on top of the Arduino Uno and connect the Arduino Uno to the Pico through the uart connection.
- Connect pin 2 of the Arduino Uno to Pin 0 of the Pico.
- Connect pin 3 of the Arduino Uno to Pin 1 of the Pico.
- Connect the ground from the Arduino Uno to the ground of the Pico.
![Connections](https://cdn.discordapp.com/attachments/713846997039448134/1199471776174985366/PicoArduinoSerial.png)
- The idea behind this, is that you send serial input to your Arduino Uno from your PC, then it will merge that with the USB Host Shield, which will take input from the mouse. It will then pass all that data along to the Pico which will act as the HID mouse.
    - You will have to unplug all mice from your PC and plug your main one into the USB Host Shield, this is because Vanguard doesn't allow multiple mice to work at the same time. This is also the entire reason you need a USB Host Shield in the first place.

## [Arduino Leonardo](https://store.arduino.cc/products/arduino-leonardo-with-headers) and a [USB Host Shield](https://github.com/felis/USB_Host_Shield_2.0)

This is by far the most popular option as it only requires 2 components, though this isn't cheap. You can get these components for very little on online Chinese stores such as AliExpress. I got an Arduino Leonardo for 1.5 USD + 2 USD shipping. If you don't want to spend much and don't really care about having to wait ~10 days then getting it on these sites is a really good alternative.

- Place the USB Host Shield on top of the Arduino Leonardo.
- The idea is that you send serial input to your Arduino Leonardo from your PC then it will merge that with the USB Host Shield which will take input from the mouse. It will then move the mouse based on the input it's given.
    - You will have to unplug all mice from your PC and plug your main one into the USB Host Shield, this is because Vanguard doesn't allow multiple mice to work at the same time. This is also the entire reason you need a USB Host Shield in the first place.

## USB Host Shield no power fix

The USB Host Shield sometimes doesn't come presoldered, there are some pads that must be soldered in order for the power to go from the Arduino to the USB port. This isn't really hard, but it is very annoying because it should just come presoldered by default.

[This](https://www.youtube.com/watch?v=1dxwU87GQVM&t=40s) is a very easy to understand and simple video on how to do this.