#pragma once
#include <Adafruit_SSD1306.h>

class Oled : public Print {
private:
    Adafruit_SSD1306 display;
public:
    Oled();
    bool init();
    void show_image(bool show);
    void clear();
    void flush();

    size_t write(uint8_t) override;

    size_t print(const String& msg);
    size_t println(const String& msg);
};