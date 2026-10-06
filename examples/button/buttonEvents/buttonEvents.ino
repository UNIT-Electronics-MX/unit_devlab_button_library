/**
 * @file buttonEvents.ino
 * @brief Prints each press and release edge of the I2C button module once.
 *
 * Uses update()/wasPressed()/wasReleased(). Compatible with ESP32, RP2040/RP2350, STM32 and AVR.
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
uint32_t pressCount = 0;

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

  if (button.update()) {
    if (button.wasPressed()) {
      pressCount++;
      Serial.print("Pressed (#");
      Serial.print(pressCount);
      Serial.println(")");
    }
    if (button.wasReleased()) Serial.println("Released");
  }
  delay(10U);
}
