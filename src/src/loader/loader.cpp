#include "loader.h"
#include "pins.h"
#include <SPI.h>
#include <SD.h>

bool Loader::init() {

    SPI.setRX(PIN_SD_MISO);
    SPI.setTX(PIN_SD_MOSI);
    SPI.setSCK(PIN_SD_SCLK);
    SPI.setCS(PIN_SD_CS);

    if (!SD.begin(PIN_SD_CS)) {
        Serial.println("[SD] FAIL - card not detected or init failed");
        return false;
    }

    return true;
}
