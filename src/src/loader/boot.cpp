#include <Arduino.h>
#include "loader/boot.h"


bool Bootloader::pressed(int pin) {
    return digitalRead(pin) == LOW;
}

void Bootloader::DO_NOT_TOUCH() {
  rp2040.begin(0);
  initVariant();
  USB.begin();
}

void Bootloader::loop(void (*main_func)(const HAL&)) {
    while (true) {
        main_func(hal);
    }
}
void Bootloader::init() {

    DO_NOT_TOUCH();

    // boot sequence
    bool oled_ok = oled.init();
    oled.show_image(true);
    delay(1000);
    oled.clear();

    Print& stream = oled;
    #ifdef LOGGING
    Serial.begin(115200);
    delay(5000);
    stream = Serial;
    stream.println("[BOOT] Initializing bootloader...");
    #endif


    bool sd_ok = loader.init();
    bool buttons_ok = buttons.init();

    if (!buttons_ok) {
        exception("[BOOT] Buttons init failed!", stream);
    }

    // test fat32
    if (!loader.testFat32()) {
      exception("[BOOT] SD card test failed! Is the card FAT32 formatted?", stream);
      sd_ok = false;
    }
    
    if (!sd_ok) {
        stream.println("[BOOT] SD card init failed!");
    }

    if (sd_ok) {
        loader.listRoot([&stream](const char* name) {
            stream.println(name);
        });
    }

    delay(1000);
    oled.clear();

}
