# Pico Installation

The Pico will act as a HID

- First hold down the BOOTSEL button on your Pico while it's unplugged, then while holding the button plug it into your PC.
    - It should come up as a USB drive with only 2 files in it.
- Move `adafruit-circuitpython-raspberry_pi_pico-en_US-8.2.9.uf2` into that drive. This will install CircuitPython on your Pico.
    - After waiting a couple seconds you should see a new drive with the name `CIRCUITPY`.
- The new drive should contain a `lib` folder and `code.py` replace these with the ones in this repository.
- Now your Pico can be used as a HID and it's able to be controlled through UART.
