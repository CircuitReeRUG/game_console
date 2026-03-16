#include "loader/sd.h"
bool SDLoader::init() {
    SPI.setRX(PIN_SD_MISO);
    SPI.setTX(PIN_SD_MOSI);
    SPI.setSCK(PIN_SD_SCLK);
    SPI.setCS(PIN_SD_CS);

    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, HIGH);
    SPI.begin();

    return true;
}

bool SDLoader::testFat32() {
    File root = SD.open("/");
    if (!root) {
        
        return false;
    }
    root.close();
    return true;
}