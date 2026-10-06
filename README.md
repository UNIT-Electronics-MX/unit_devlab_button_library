# DevLab_Button

Arduino library for the I2C push-button module using the DevLab Device
Protocol (DDP) over I2C.

This library wraps the shared
[`DevLabDDP`](https://github.com/UNIT-Electronics-MX/unit_devlab_ddp_library)
master and the
[`DevLab_Interface`](https://github.com/UNIT-Electronics-MX/unit_devlab_interface_library)
`DevLab_I2C_Orchestrator` bus class into a single `DevLab_Button` object.
Firmware: `unit_firmware_i2c_push_button_module`.

Compatible with ESP32 and RP2040/RP2350.

# Features

- Device identification (Device ID `0x0110`) before any command
- `readPressed()` reports the debounced button state (firmware debounce, 5 ms sampling)
- `update()` with `wasPressed()` / `wasReleased()` edge detection
- I2C address scan and reassignment (`0x08` to `0x77`), default address `0x30`
- Bus recovery (`beginRecovered`) for a slave left mid-transaction

# Quick Start Example

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <DevLab_Button.h>

DevLab_Button button(Wire, DevLab_Button::DEFAULT_ADDRESS, 400000);

void setup() {
  Serial.begin(115200);
  // ESP32: 6/7. RP2040/RP2350: 24/25 (Pulsar) or 12/13.
  if (!button.beginRecovered(6, 7)) {
    Serial.println("Button module not found");
  }
}

void loop() {
  if (button.update()) {
    if (button.wasPressed())  Serial.println("Pressed");
    if (button.wasReleased()) Serial.println("Released");
  }
  delay(10);
}
```

# API

| Method | Description |
|---|---|
| `DevLab_Button(wire, address, clock)` | Create the object (default `Wire`, `0x30`, 400 kHz). |
| `begin()` / `begin(sda, scl, clock)` | Start the bus and verify the device. |
| `beginRecovered(sda, scl, timeoutUs, restart)` | Same, clearing a stuck bus first. |
| `readPressed(pressed)` | `true` while the button is held. |
| `update()` | Read once and keep the previous state; `false` on I2C error. |
| `isPressed()` / `wasPressed()` / `wasReleased()` | State and edges between two `update()` calls. |
| `isConnected()` / `busReady()` | Result of the last `begin`. |
| `deviceInfo()` / `printInfo(out)` | Identity reported by the module. |
| `protocol()` / `bus()` | Access the underlying DDP master and I2C bus. |

All calls return `false` on an I2C error or if `begin` has not verified the
device. The button (S2, PA2) is read with DDP command `0x80`: one byte, bit 0 =
pressed, bits 7..1 reserved.

# License

MIT, see [LICENSE](LICENSE).
