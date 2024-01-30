#include "HID-Project.h"

uint8_t rawhidData[255];

void setup() {
  RawHID.begin(rawhidData, sizeof(rawhidData));
}

void loop() {
    // Check if something has been received
    if (RawHID.available() > 0) {
        switch (RawHID.read()) {
        case 'C': // Click event
            switch (RawHID.read()) {
            case '1': Mouse.click(MOUSE_LEFT); break;
            case '2': Mouse.click(MOUSE_RIGHT); break;
            case '3': Mouse.click(MOUSE_MIDDLE); break;
            case '4': Mouse.click(MOUSE_PREV); break;
            case '5': Mouse.click(MOUSE_NEXT); break;
            default:
                break;
            }
            break;
        case 'P': // Press event
            switch (RawHID.read()) {
            case '1': Mouse.press(MOUSE_LEFT); break;
            case '2': Mouse.press(MOUSE_RIGHT); break;
            case '3': Mouse.press(MOUSE_MIDDLE); break;
            case '4': Mouse.press(MOUSE_PREV); break;
            case '5': Mouse.press(MOUSE_NEXT); break;
            default:
                break;
            }
            break;
        case 'R': // Release event
            switch (RawHID.read()) {
            case '1': Mouse.release(MOUSE_LEFT); break;
            case '2': Mouse.release(MOUSE_RIGHT); break;
            case '3': Mouse.release(MOUSE_MIDDLE); break;
            case '4': Mouse.release(MOUSE_PREV); break;
            case '5': Mouse.release(MOUSE_NEXT); break;
            case 'A': Mouse.releaseAll(); break;
            default:
                break;
            }
            break;
        case 'M': // Move event
        {
            int x = RawHID.read();
            int y = RawHID.read();
            Mouse.move(x, y);
            break;
        }
        default:
            break;
        }
    }
}