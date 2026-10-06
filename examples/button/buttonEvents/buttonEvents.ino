/**
 * @file buttonEvents.ino
 * @brief Prints each press and release edge of the I2C button module once.
 *
 * Uses update()/wasPressed()/wasReleased(). Compatible with ESP32 and RP2040/RP2350.
 *
 * @author Jonathan Mejorado
 * @organization UNIT Electronics MX
 */

#include <Arduino.h>
#include <Wire.h>
#include <DevLab_Button.h>

#if defined(ARDUINO_ARCH_RP2040)
  #if defined(PIN_WIRE1_SDA) && defined(PIN_WIRE1_SCL) && \
      PIN_WIRE1_SDA == 24U && PIN_WIRE1_SCL == 25U
    constexpr uint8_t I2C_SDA = 24U, I2C_SCL = 25U;
  #else
    constexpr uint8_t I2C_SDA = 12U, I2C_SCL = 13U;
  #endif
  constexpr uint32_t I2C_CLOCK_HZ = 100000U;
#elif defined(ARDUINO_ARCH_ESP32)
  constexpr uint8_t I2C_SDA = 6U, I2C_SCL = 7U;
  constexpr uint32_t I2C_CLOCK_HZ = 400000U;
#else
  #error "Use ESP32 or RP2040/RP2350"
#endif

DevLab_Button button(Wire, DevLab_Button::DEFAULT_ADDRESS, I2C_CLOCK_HZ);
uint32_t pressCount = 0;

void setup() {
  Serial.begin(115200);
  delay(500);
  if (!button.beginRecovered(I2C_SDA, I2C_SCL)) {
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
