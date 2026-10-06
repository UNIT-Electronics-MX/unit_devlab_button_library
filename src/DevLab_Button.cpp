#include "DevLab_Button.h"

bool DevLab_Button::begin() {
    _verified = false;
    _bus.setClock(_clock);
    _busReady = _bus.begin();
    if (!_busReady) return false;
    return (_verified = _ddp.matchesExpectedDevice(_address, &_info));
}

bool DevLab_Button::begin(uint8_t sdaPin, uint8_t sclPin, uint32_t clock) {
    _verified = false;
    _clock = clock;
    _bus.setClock(_clock);
    _busReady = _bus.begin(sdaPin, sclPin);
    if (!_busReady) return false;
    return (_verified = _ddp.matchesExpectedDevice(_address, &_info));
}

bool DevLab_Button::beginRecovered(uint8_t sdaPin, uint8_t sclPin, uint32_t timeoutUs, bool restart) {
    _verified = false;
    _bus.setClock(_clock);
    _busReady = _bus.beginRecovered(sdaPin, sclPin, timeoutUs, restart);
    if (!_busReady) return false;
    return (_verified = _ddp.matchesExpectedDevice(_address, &_info));
}

void DevLab_Button::printInfo(Print &out) const {
    DevLabDDP::printDeviceInfo(out, _address, _info, _ddp.expectedDeviceId());
}

bool DevLab_Button::readPressed(bool &pressed) {
    if (!_verified) return false;
    uint8_t raw = 0U;
    if (!_ddp.readCommand(_address, CMD_READ_BUTTON, &raw, 1U, 2U) ||
        (raw & 0xFEU) != 0U) return false;
    pressed = (raw & S2) != 0U;
    return true;
}

bool DevLab_Button::update() {
    bool pressed;
    if (!readPressed(pressed)) return false;
    _previous = _state;
    _state = pressed;
    return true;
}
