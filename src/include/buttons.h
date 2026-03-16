#pragma once

#include <stdint.h>
#include "pins.h"

class Buttons {
public:
    bool init();
    const char* read_clicked();

private:
    uint8_t prev_state_mask = 0;
};


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
