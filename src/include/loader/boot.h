#pragma once
#include <stdint.h>
#include "buttons.h"
#include "sd.h"
#include "oled.h"
#include <USB.h>

struct KeyState {
    bool up;
    bool down;
    bool left;
    bool right;
    bool a;
    bool b;
};

struct HAL {
    void (*draw_pixel)(int x, int y, uint16_t whiteness);
    KeyState (*read_input)();
};

class Bootloader {
    void DO_NOT_TOUCH();
    HAL hal;

    Oled oled;
    SDLoader loader;
    Buttons buttons;
    
public:
    bool pressed(int pin);
    void init();
    void loop(void (*main_func)(const HAL&));
    HAL get_hal() const {
        return hal;
    }
};