#ifndef DEVLAB_BUTTON_H
#define DEVLAB_BUTTON_H

#pragma once

#include "DevLabDDP.h"
#include "DevLabDDPConsole.h"
#include "DevLab_I2C_Orchestrator.h"

class DevLab_Button
{
public:
    /* DDP Device ID of the push-button module (DDP_DEVICE_ID in firmware). */
    static constexpr uint16_t DEVICE_ID = 0x0110U;

    /* Factory I2C address of the button module (STARTUP_I2C_ADDRESS). */
    static constexpr uint8_t DEFAULT_ADDRESS = 0x30U;

    /* Button mask (1 = pressed). Button S2 is wired to PA2 on the module. */
    static constexpr uint8_t S2 = 1U << 0;

    explicit DevLab_Button(TwoWire &wire = Wire, uint8_t address = DEFAULT_ADDRESS, uint32_t clock = 400000UL)
    : _bus(wire, clock), _ddp(_bus, DEVICE_ID), _address(address), _clock(clock) {}

    bool begin();
    bool begin(uint8_t sdaPin, uint8_t sclPin, uint32_t clock = 400000UL);
    bool beginRecovered(uint8_t sdaPin, uint8_t sclPin, uint32_t timeoutUs = 20000, bool restart = false);

    /* Reads the debounced button state (command 0x80). `pressed` is true
     * while the button is held. Returns false on an I2C error or if the
     * frame is invalid (bits 7..1 are reserved and always 0). */
    bool readPressed(bool &pressed);

    /* Polling helpers: update() reads the module once and keeps the previous
     * state, so wasPressed()/wasReleased() report edges between two update()
     * calls. Returns false on an I2C error (state is left unchanged). */
    bool update();
    bool isPressed() const { return _state; }
    bool wasPressed() const { return _state && !_previous; }
    bool wasReleased() const { return !_state && _previous; }

    bool busReady() const { return _busReady; }
    bool isConnected() const { return _verified; }
    uint8_t address() const { return _address; }
    const DevLabDDP::DeviceInfo &deviceInfo() const { return _info; }
    void printInfo(Print &out = Serial) const;

    DevLabDDP::Master &protocol() { return _ddp; }
    DevLab_I2C_Orchestrator &bus() { return _bus; }

private:
    static constexpr uint8_t CMD_READ_BUTTON = 0x80U;

    DevLab_I2C_Orchestrator _bus;
    DevLabDDP::Master _ddp;
    uint8_t _address;
    uint32_t _clock;
    bool _busReady = false;
    bool _verified = false;
    bool _state = false;
    bool _previous = false;
    DevLabDDP::DeviceInfo _info;
};

#endif
