#include "buttons.h"
#include "pins.h"
#include <Arduino.h>

struct Button {
    const char* name;
    uint8_t pin;
};

static const Button BUTTONS[] = {
    {"A",     PIN_BTN_A},
    {"B",     PIN_BTN_B},
    {"UP",    PIN_UP},
    {"DOWN",  PIN_DOWN},
    {"LEFT",  PIN_LEFT},
    {"RIGHT", PIN_RIGHT},
};

void Buttons::init() {
    for (auto& btn : BUTTONS) {
        pinMode(btn.pin, INPUT_PULLUP);
    }
    Serial.println("[BUTTONS] Initialized");
}
