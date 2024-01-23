# Arduino Uno Installation

I haven't finished the code for this yet, because my USB Host Shield hasn't arrived yet.

This is the only code I have so far, which just acts as a USB to UART converter (yes I know you can ground the reset pin)

```cpp
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);  // RX, TX

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
}

void loop() {
  if (Serial.available() > 0) {
    mySerial.write(Serial.read());
  }
}
```
