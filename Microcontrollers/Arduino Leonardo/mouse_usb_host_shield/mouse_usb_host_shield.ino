#include <hidboot.h>
#include <usbhub.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include "HID-Project.h"   // Native USB HID mouse emulation + RawHID

// ---------- Mouse report layout (8 bytes from the host-shield mouse) ----------
struct MouseInfo {
  struct {
    uint8_t bmLeftButton   : 1;
    uint8_t bmRightButton  : 1;
    uint8_t bmMiddleButton : 1;
    uint8_t bmDummy        : 5;
  };
  uint8_t padding;
  int16_t dX;      // + right, - left
  int16_t dY;      // + down,  - up
  int16_t dWheel;  // + up,    - down
};
static_assert(sizeof(MouseInfo) == 8, "MouseInfo must be 8 bytes");

void printHex(uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    if (data[i] < 0x10) Serial.print("0");
    Serial.print(data[i], HEX);
  }
  Serial.println();
}

// ================= USB Host Shield: physical mouse -> PC =================
class MouseParser : public HIDReportParser {
  union {
    MouseInfo mouseInfo;
    uint8_t   bInfo[sizeof(MouseInfo)];
  } prevState;

public:
  void OnMouseMove(MouseInfo *mi) {
    // Relay movement to the PC. Wheel is included in the same call.
    Mouse.move(mi->dX, mi->dY, mi->dWheel);

    Serial.print("Move dx=");
    Serial.print(mi->dX, DEC);
    Serial.print(" dy=");
    Serial.print(mi->dY, DEC);
    Serial.print(" wheel=");
    Serial.println(mi->dWheel, DEC);
  }

  void OnLeftButtonUp()    { Mouse.release(MOUSE_LEFT);   Serial.println("L Up");   }
  void OnLeftButtonDown()  { Mouse.press(MOUSE_LEFT);     Serial.println("L Dn");   }
  void OnRightButtonUp()   { Mouse.release(MOUSE_RIGHT);  Serial.println("R Up");   }
  void OnRightButtonDown() { Mouse.press(MOUSE_RIGHT);    Serial.println("R Dn");   }
  void OnMiddleButtonUp()  { Mouse.release(MOUSE_MIDDLE); Serial.println("M Up");   }
  void OnMiddleButtonDown(){ Mouse.press(MOUSE_MIDDLE);   Serial.println("M Dn");   }

  void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
    if (len < sizeof(MouseInfo)) return;

    MouseInfo *pmi = (MouseInfo *)buf;

    if (prevState.mouseInfo.bmLeftButton != pmi->bmLeftButton) {
      pmi->bmLeftButton ? OnLeftButtonDown() : OnLeftButtonUp();
    }
    if (prevState.mouseInfo.bmRightButton != pmi->bmRightButton) {
      pmi->bmRightButton ? OnRightButtonDown() : OnRightButtonUp();
    }
    if (prevState.mouseInfo.bmMiddleButton != pmi->bmMiddleButton) {
      pmi->bmMiddleButton ? OnMiddleButtonDown() : OnMiddleButtonUp();
    }

    // Movement and/or wheel -> single Mouse.move() call carries both.
    if (pmi->dX || pmi->dY || pmi->dWheel) {
      OnMouseMove(pmi);
    }

    memcpy(prevState.bInfo, buf, sizeof(MouseInfo));
  }
};

USB Usb;
USBHub Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse(&Usb);
MouseParser Prs;

// ================= RawHID: PC -> emulated mouse =================
uint8_t rawhidData[255];

void handleRawHID() {
  while (RawHID.available() > 0) {
    switch (RawHID.read()) {
      case 'C': // Click
        switch (RawHID.read()) {
          case '1': Mouse.click(MOUSE_LEFT);   break;
          case '2': Mouse.click(MOUSE_RIGHT);  break;
          case '3': Mouse.click(MOUSE_MIDDLE); break;
          case '4': Mouse.click(MOUSE_PREV);   break;
          case '5': Mouse.click(MOUSE_NEXT);   break;
        }
        break;

      case 'P': // Press
        switch (RawHID.read()) {
          case '1': Mouse.press(MOUSE_LEFT);   break;
          case '2': Mouse.press(MOUSE_RIGHT);  break;
          case '3': Mouse.press(MOUSE_MIDDLE); break;
          case '4': Mouse.press(MOUSE_PREV);   break;
          case '5': Mouse.press(MOUSE_NEXT);   break;
        }
        break;

      case 'R': // Release
        switch (RawHID.read()) {
          case '1': Mouse.release(MOUSE_LEFT);   break;
          case '2': Mouse.release(MOUSE_RIGHT);  break;
          case '3': Mouse.release(MOUSE_MIDDLE); break;
          case '4': Mouse.release(MOUSE_PREV);   break;
          case '5': Mouse.release(MOUSE_NEXT);   break;
          case 'A': Mouse.releaseAll();          break;
        }
        break;

      case 'M': // Move: 'M' <int8 x> <int8 y> <int8 wheel>
      {
        int x = (int8_t)RawHID.read();
        int y = (int8_t)RawHID.read();
        int w = (int8_t)RawHID.read();
        Mouse.move(x, y, w);
        break;
      }

      case 'W': // Wheel only: 'W' <int8 wheel>   (optional convenience)
      {
        int w = (int8_t)RawHID.read();
        Mouse.move(0, 0, w);
        break;
      }

      default:
        break;
    }
  }
}

// ================= Setup / Loop =================
void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial)
    ;
#endif
  Serial.println("Start");

  Mouse.begin();
  RawHID.begin(rawhidData, sizeof(rawhidData));

  if (Usb.Init() == -1)
    Serial.println("OSC did not start.");

  delay(200);

  HidMouse.SetReportParser(0, &Prs);
}

void loop() {
  handleRawHID();
  Usb.Task();
}