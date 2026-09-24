import busio
import board
import usb_hid
from adafruit_hid.mouse import Mouse

mouse = Mouse(usb_hid.devices)
uart = busio.UART(tx=board.GP0, rx=board.GP1, baudrate=115200)

def parse(message):
    print(message)
    if message == "FIRE":
        mouse.click(Mouse.LEFT_BUTTON)
    else:
        # If messages are sent too fast, sometimes it will miss a \n which will make it crash
        try:
            x, y = message.split(",")
        except:
            return

        try:
            x = int(x)
            y = int(y)
        except ValueError:
            print("Error: " + message)
            return
        
        mouse.move(x, y)

buffer = ""
while True:
    if uart.in_waiting:
        data = uart.read(1)
        try:
            data = data.decode()
        except:
            print(data)
            continue
        
        # Every message ends with a \n so parse the buffer and reset it
        if data == '\n':
            parse(buffer)
            buffer = ""
        else:
            buffer += data