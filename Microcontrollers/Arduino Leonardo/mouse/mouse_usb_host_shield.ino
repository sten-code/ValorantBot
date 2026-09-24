#include <hidboot.h>
#include <usbhub.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

#include "HID-Project.h"

struct MouseInfo {
  struct {
    uint8_t bmLeftButton   : 1;
    uint8_t bmRightButton  : 1;
    uint8_t bmMiddleButton : 1;
    uint8_t bmDummy        : 5;
  };
  uint8_t padding1;
  int16_t dX;
  int16_t dY;
  int8_t dWheel;
  uint8_t padding2;
};
static_assert(sizeof(MouseInfo) == 8, "MouseInfo must be 8 bytes");

class MouseParser : public HIDReportParser {
  union {
    MouseInfo mouseInfo;
    uint8_t   bInfo[sizeof(MouseInfo)];
  } prevState;

public:
  void OnMouseMove(MouseInfo *mi) {
    Mouse.move(mi->dX, mi->dY, mi->dWheel);
  }

  void OnLeftButtonUp()    { Mouse.release(MOUSE_LEFT);   }
  void OnLeftButtonDown()  { Mouse.press(MOUSE_LEFT);     }
  void OnRightButtonUp()   { Mouse.release(MOUSE_RIGHT);  }
  void OnRightButtonDown() { Mouse.press(MOUSE_RIGHT);    }
  void OnMiddleButtonUp()  { Mouse.release(MOUSE_MIDDLE); }
  void OnMiddleButtonDown(){ Mouse.press(MOUSE_MIDDLE);   }

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

      case 'M':
      {
        int x = (int8_t)RawHID.read();
        int y = (int8_t)RawHID.read();
        int w = (int8_t)RawHID.read();
        Mouse.move(x, y, w);
        break;
      }

      case 'W':
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

void setup() {
  Mouse.begin();
  RawHID.begin(rawhidData, sizeof(rawhidData));

  Usb.Init();

  delay(200);

  HidMouse.SetReportParser(0, &Prs);
}

void loop() {
  handleRawHID();
  Usb.Task();
}