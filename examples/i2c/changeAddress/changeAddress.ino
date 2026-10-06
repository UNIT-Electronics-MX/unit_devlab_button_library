#include <Arduino.h>
#include <Wire.h>
#include <DevLabDDP.h>
#include <DevLab_Button.h>
#include <DevLab_I2C_Orchestrator.h>

#if defined(ARDUINO_ARCH_RP2040)
  /* Both pin pairs below use I2C0, exposed as Wire by these variants. */
  #define I2C_BUS Wire
  #if defined(PIN_WIRE1_SDA) && defined(PIN_WIRE1_SCL) && \
      PIN_WIRE1_SDA == 24U && PIN_WIRE1_SCL == 25U
    /* Pulsar connector pins. Core 3.1.0 incorrectly assigns them to Wire1. */
    constexpr uint8_t I2C_SDA = 24U, I2C_SCL = 25U;
  #else
    /* Generic RP2040/RP2350 wiring. */
    constexpr uint8_t I2C_SDA = 12U, I2C_SCL = 13U;
  #endif
  constexpr uint32_t I2C_CLOCK_HZ = 100000U;
#elif defined(ARDUINO_ARCH_ESP32)
  #define I2C_BUS Wire
  constexpr uint8_t I2C_SDA = 6U, I2C_SCL = 7U;
  constexpr uint32_t I2C_CLOCK_HZ = 400000U;
#else
  #error "Use ESP32 or RP2040/RP2350"
#endif

constexpr uint16_t EXPECTED_DEVICE_ID = DevLab_Button::DEVICE_ID;
DevLab_I2C_Orchestrator bus(I2C_BUS, I2C_CLOCK_HZ);
DevLabDDP::Master master(bus, EXPECTED_DEVICE_ID);
String inputLine;

bool parseAddress(const String &text, uint8_t &address) {
  char *end = nullptr;
  long value = strtol(text.c_str(), &end, 16);
  if (end == text.c_str() || *end != '\0' || value < 0x08L || value > 0x77L) {
    return false;
  }
  address = (uint8_t)value;
  return true;
}

void printHexAddress(uint8_t address) {
  Serial.print("0x");
  if (address < 0x10U) Serial.print('0');
  Serial.print(address, HEX);
}

void scanBus() {
  bool found = false;
  Serial.println("Address  Sensor");
  for (uint8_t address = 0x08U; address <= 0x77U; ++address) {
    if (!master.ping(address)) continue;
    found = true;
    printHexAddress(address);
    Serial.print("     ");
    DevLabDDP::DeviceInfo info;
    Serial.println(master.identify(address, info)
                       ? DevLabDDP::deviceName(info.deviceId)
                       : "non-DDP");
  }
  if (!found) Serial.println("--       none");
}

void printHelp() {
  Serial.println("Commands:");
  Serial.println("  scan");
  Serial.println("  change <current_hex> <new_hex>");
  Serial.println("Example:");
  Serial.println("  change 28 30");
}

void processCommand(String line) {
  line.trim();
  line.toLowerCase();

  if (line == "scan") {
    scanBus();
    return;
  }

  int firstSpace = line.indexOf(' ');
  int secondSpace = firstSpace < 0 ? -1 : line.indexOf(' ', firstSpace + 1);
  if (firstSpace < 0 || secondSpace < 0 ||
      line.substring(0, firstSpace) != "change") {
    Serial.println("ERROR invalid command");
    printHelp();
    return;
  }

  String oldText = line.substring(firstSpace + 1, secondSpace);
  String newText = line.substring(secondSpace + 1);
  oldText.trim();
  newText.trim();

  uint8_t oldAddress, newAddress;
  if (!parseAddress(oldText, oldAddress) ||
      !parseAddress(newText, newAddress) ||
      oldAddress == newAddress) {
    Serial.println("ERROR addresses must be different hexadecimal values from 08 to 77");
    return;
  }

  DevLabDDP::DeviceInfo info;
  if (!master.matchesExpectedDevice(oldAddress, &info)) {
    Serial.println("ERROR current address does not contain the expected DDP device");
    return;
  }
  if (master.ping(newAddress)) {
    Serial.println("ERROR new address is already in use");
    return;
  }

  Serial.print("Changing ");
  printHexAddress(oldAddress);
  Serial.print(" -> ");
  printHexAddress(newAddress);
  Serial.println("...");

  if (!master.setI2cAddress(oldAddress, newAddress)) {
    Serial.println("ERROR address change failed");
    return;
  }

  Serial.print("OK device is now available at ");
  printHexAddress(newAddress);
  Serial.println();
  scanBus();
}

void setup() {
  Serial.begin(115200);
  delay(500);

  bus.beginRecovered(I2C_SDA, I2C_SCL);

  Serial.print("Expected DDP device ID: 0x");
  Serial.println(EXPECTED_DEVICE_ID, HEX);
  printHelp();
  scanBus();
}

void loop() {
  while (Serial.available()) {
    char character = (char)Serial.read();
    if (character == '\r' || character == '\n') {
      if (inputLine.length() > 0U) {
        processCommand(inputLine);
        inputLine = "";
      }
    } else if (inputLine.length() < 64U) {
      inputLine += character;
    } else {
      inputLine = "";
      Serial.println("ERROR command is too long");
    }
  }
}
