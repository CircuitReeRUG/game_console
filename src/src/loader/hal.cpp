#include "loader/boot.h"

volatile bool game_running = false;

namespace {
Oled* g_oled = nullptr;
}

HAL Bootloader::create_hal() {
  g_oled = &oled;

  HAL h = {};

  h.draw_pixel = [](int x, int y, uint16_t color) {
    g_oled->drawPixel(x, y, color);
  };

  h.clear_screen = []() {
    g_oled->clear();
  };

  h.render = []() {
    g_oled->render();
  };

  h.read_input = []() -> KeyState {
    return {
        digitalRead(PIN_UP) == LOW,
        digitalRead(PIN_DOWN) == LOW,
        digitalRead(PIN_LEFT) == LOW,
        digitalRead(PIN_RIGHT) == LOW,
        digitalRead(PIN_BTN_A) == LOW,
        digitalRead(PIN_BTN_B) == LOW
    };
  };

  h.delay_ms = [](uint32_t ms) {
    delay(ms);
  };

  h.screen_width = 128;
  h.screen_height = 64;

  return h;
}
