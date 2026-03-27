#include "loader/boot.h"

volatile bool gameRunning = false;

namespace {
Oled* gOled = nullptr;
// needed to put it in the namespace so it can be used in drawString
void drawChar(int x, int y, char c, uint16_t color, uint8_t font[96][6]) {
  // Convert the character to an index
  c = c & 0x7F;
  if (c < ' ') {
    c = 0;
  } else {
    c -= ' ';
  }

  // 'font' is a multidimensional array of [96][6]
  // which is really just a 1D array of size 96*6.
  const uint8_t* chr = font[c*6];
  for (uint8_t j=0; j<6; j++) {
    for (uint8_t i=0; i<8; i++) {
      if (chr[j] & (1<<i)) {
        gOled->drawPixel(x+j, y+i, color);
      }
    }
  }
};
}

HAL Bootloader::createHal() {
  gOled = &oled;

  HAL h = {};

  h.drawPixel = [](int x, int y, uint16_t color) {
    gOled->drawPixel(x, y, color);
  };
  h.drawChar = &drawChar;

  h.drawText = [](int x, int y, const char* text, uint16_t color, uint8_t font[96][6]) {
    while (*text) {
      drawChar(*text++, x, y, color, font);
      x += 6;
    }
  };

  h.clearScreen = []() {
    gOled->clear();
  };

  h.render = []() {
    gOled->render();
  };

  h.readInput = []() -> KeyState {
    return {
        digitalRead(PIN_UP) == LOW,
        digitalRead(PIN_DOWN) == LOW,
        digitalRead(PIN_LEFT) == LOW,
        digitalRead(PIN_RIGHT) == LOW,
        digitalRead(PIN_BTN_A) == LOW,
        digitalRead(PIN_BTN_B) == LOW
    };
  };

  h.delayMs = [](uint32_t ms) {
    delay(ms);
  };

  h.sdRead = [](const char* path, uint8_t* buf, uint32_t offset, uint32_t len) -> int {
    File f = SD.open(path, FILE_READ);
    if (!f)
      return -1;
    if (!f.seek(offset)) { 
      f.close();
      return -1;
    }
    int n = f.read(buf, len);
    f.close();
    return n;
  };

  h.sdWrite = [](const char* path, const uint8_t* buf, uint32_t len) -> int {
    File f = SD.open(path, FILE_WRITE);
    if (!f)
      return -1;
    int n = f.write(buf, len);
    f.close();
    return n;
  };
  
  h.getTime = []() -> absolute_time_t {
    return get_absolute_time();
  };

  h.screenWidth = 128;
  h.screenHeight = 64;

  return h;
}
