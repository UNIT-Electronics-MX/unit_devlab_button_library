# DevLab_Button examples

| Example | Purpose |
|---|---|
| `button/readButton` | Print "Pressed" / "Released" whenever the button state changes. |
| `button/buttonEvents` | Print each press (with a counter) and release edge once, using `update()`. |
| `i2c/changeAddress` | Scan the bus and change the I2C address of a button module (default `0x30`). |

All examples verify Device ID `0x0110` before sending any command. The button
state is read with command `0x80`, which returns one byte: bit 0 is S2 (PA2),
1 = pressed; bits 7..1 are always 0 (a frame with those bits set is rejected).
The firmware samples the pin every 5 ms and debounces it over 4 samples.

Wiring (`Wire`):

| Master | SDA | SCL |
|---|---|---|
| ESP32 | GPIO6 | GPIO7 |
| Pulsar RP2350A | GPIO24 | GPIO25 |
| Other RP2040 / RP2350 | GPIO12 | GPIO13 |

Connect the master's GND to the module's GND and use Serial at 115200 baud.
To change the address, open Serial and enter `scan`, then e.g. `change 30 31`.
