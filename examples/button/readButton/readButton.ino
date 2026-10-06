/**
 * @file readButton.ino
 * @brief Prints "Pressed" / "Released" when the I2C button module changes state.
 *
 * Compatible with ESP32, RP2040/RP2350, STM32 and AVR.
 *
 * @author Jonathan Mejorado
 * @organization UNIT Electronics MX
 */

#include <Arduino.h>
#include <Wire.h>
#include <DevLab_Button.h>

#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350)
  #define I2C_BUS Wire1
  #if defined(PIN_WIRE1_SDA) && defined(PIN_WIRE1_SCL) && \
      PIN_WIRE1_SDA == 24U && PIN_WIRE1_SCL == 25U
    // Pulsar connector pins.
    constexpr int SDA_PIN = 24;
    constexpr int SCL_PIN = 25;
  #else
    // Generic RP2040/RP2350 wiring.
    constexpr int SDA_PIN = 12;
    constexpr int SCL_PIN = 13;
  #endif
  constexpr uint32_t I2C_FREQ = 400000UL;
#elif defined(ARDUINO_ARCH_ESP32)
  #define I2C_BUS Wire
  constexpr int SDA_PIN = 6;
  constexpr int SCL_PIN = 7;
  constexpr uint32_t I2C_FREQ = 400000UL;
#elif defined(ARDUINO_ARCH_STM32)
  // STM32duino: default I2C pins of the selected board.
  #define I2C_BUS Wire
  constexpr int SDA_PIN = SDA;
  constexpr int SCL_PIN = SCL;
  constexpr uint32_t I2C_FREQ = 400000UL;
#elif defined(ARDUINO_ARCH_AVR)
  // AVR has fixed I2C pins (Uno/Nano: A4/A5, Mega: 20/21, Leonardo: 2/3);
  // SDA/SCL come from the board variant and begin() ignores the pin numbers.
  #define I2C_BUS Wire
  constexpr int SDA_PIN = SDA;
  constexpr int SCL_PIN = SCL;
  constexpr uint32_t I2C_FREQ = 100000UL;  // 400 kHz falla con el level shifter en UNO
#else
#error "Use an ESP32, RP2040, RP2350, STM32, or AVR master"
#endif

DevLab_Button button(I2C_BUS, DevLab_Button::DEFAULT_ADDRESS, I2C_FREQ);
bool last_pressed = false;
bool first_sample = true;

void setup() {
  Serial.begin(115200);
  delay(500);
  if (!button.beginRecovered(SDA_PIN, SCL_PIN)) {
    Serial.println("Button module not found; check wiring, power and firmware");
    return;
  }
  button.printInfo(Serial);
}

void loop() {
  if (!button.isConnected()) return;

  bool pressed = false;
  if (!button.readPressed(pressed)) {
    Serial.println("ERROR: button read failed");
    delay(500U);
    return;
  }
  if (first_sample || pressed != last_pressed) {
    Serial.println(pressed ? "Pressed" : "Released");
    last_pressed = pressed;
    first_sample = false;
  }
  delay(25U);
}
