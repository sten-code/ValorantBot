import usb.core

def list_usb_devices():
    # Find all USB devices
    usb.core.Device
    devices = usb.core.find(find_all=True)

    if devices is None:
        print("No USB devices found.")
        return

    print("List of connected USB devices:")
    print("--------------------------------")
    
    for device in devices:
        vid = hex(device.idVendor)
        pid = hex(device.idProduct)
        try:
            product = device.product
        except:
            continue
        print(f"{product}: Vendor ID (VID): {vid}, Product ID (PID): {pid}, Serial: {device.serial_number}")
    
    print("--------------------------------")

if __name__ == "__main__":
    list_usb_devices()
