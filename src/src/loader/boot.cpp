#include <Arduino.h>
#include <vector>
#include <Adafruit_TinyUSB.h>
#include "loader/boot.h"

constexpr uint32_t RP2040_SRAM_END = 0x20040000;
constexpr size_t MAX_GAME_SIZE = RP2040_SRAM_END - GAME_LOAD_ADDR;
constexpr uint16_t SD_BLOCK_SIZE = 512;

Adafruit_USBD_MSC usb_msc;
Sd2Card usb_msc_card;
SdVolume usb_msc_volume;
bool usb_msc_started = false;

// necessary functions for the USB MSC
int32_t msc_read_cb(uint32_t lba, void* buffer, uint32_t bufsize) {
    (void)bufsize;
    return usb_msc_card.readBlock(lba, static_cast<uint8_t*>(buffer)) ? SD_BLOCK_SIZE : -1;
}

int32_t msc_write_cb(uint32_t lba, uint8_t* buffer, uint32_t bufsize) {
    (void)bufsize;
    return usb_msc_card.writeBlock(lba, buffer) ? SD_BLOCK_SIZE : -1;
}

void msc_flush_cb(void) {
}

void massStorage() {
    if (usb_msc_started) {
        return;
    }

    usb_msc.setID("GameCon", "SD Upload", "1.0");
    usb_msc.setReadWriteCallback(msc_read_cb, msc_write_cb, msc_flush_cb);
    usb_msc.setUnitReady(false);
    usb_msc.begin();

    if (TinyUSBDevice.mounted()) {
        TinyUSBDevice.detach();
        delay(10);
        TinyUSBDevice.attach();
    }

    usb_msc_started = true;
}

bool isGameFile(const char* name) {
    if (!name) {
        return false;
    }

    String filename(name);
    filename.toLowerCase();
    return filename.endsWith(".bin");
}

String gamePath(const char* name) {
    if (!name || name[0] == '\0') {
        return String();
    }
    if (name[0] == '/') {
        return String(name);
    }
    return String("/") + name;
}

std::vector<String> findGames(SDLoader& loader) {
    std::vector<String> games;
    loader.listRoot([&games](const char* name) {
        if (isGameFile(name)) {
            games.push_back(String(name));
        }
    });
    return games;
}

void drawMenu(Oled& oled, const std::vector<String>& items, size_t selected) {
    oled.clear();
    oled.println("Menu");
    oled.println("A=Select B=Rescan");

    if (items.empty()) {
        oled.println("No .bin files found");
        oled.render();
        return;
    }

    size_t start = 0;
    if (selected >= 3) {
        start = selected - 2;
    }
    size_t end = start + 4;
    if (end > items.size()) {
        end = items.size();
    }

    for (size_t i = start; i < end; ++i) {
        oled.print(i == selected ? "> " : "  ");
        oled.println(items[i]);
    }

    oled.render();
}

bool Bootloader::pressed(int pin) {
    return digitalRead(pin) == LOW;
}

void Bootloader::DO_NOT_TOUCH() {
  rp2040.begin(0);
  initVariant();
}

void Bootloader::launch(const char* path) {
    String full_path = game_path(path);

    File f = SD.open(full_path.c_str(), FILE_READ);
    if (!f) {
        exception("[BOOT] Failed to open game file", oled);
    }

    size_t game_size = f.size();
    if (game_size == 0) {
        exception("[BOOT] Game file is empty", oled);
    }

    if (game_size > MAX_GAME_SIZE) {
        exception("[BOOT] Game too large for RAM", oled);
    }

    uint8_t* dest = (uint8_t*)GAME_LOAD_ADDR;
    memset(dest, 0, MAX_GAME_SIZE);
    while (f.available()) *dest++ = f.read();
    f.close();

    // barrier
    __dmb();

    oled.clear();
    oled.render();

    game_running = true;

    typedef void (*game_entry_t)(const HAL*);
    ((game_entry_t)(GAME_LOAD_ADDR | 1u))(&hal);

    game_running = false;
}

void Bootloader::uploadMode() {
    massStorage();

    SD.end();

    bool card_ok = usb_msc_card.init(SPI_HALF_SPEED, PIN_SD_CS);
    bool volume_ok = card_ok && usb_msc_volume.init(usb_msc_card);

    oled.clear();
    oled.println("Upload Mode");

    if (volume_ok) {
        uint32_t block_count = usb_msc_volume.blocksPerCluster() * usb_msc_volume.clusterCount();
        usb_msc.setCapacity(block_count, SD_BLOCK_SIZE);
        usb_msc.setUnitReady(true);

        oled.println("SD is USB drive.");
        oled.println("Copy .bin files.");
        oled.println("Press B to exit.");
    } else {
        usb_msc.setUnitReady(false);
        exception("[BOOT] Failed to initialize SD card for USB mass storage", oled);
    }

    oled.render();

    bool prev_b = false;
    while (true) {
        bool b = pressed(PIN_BTN_B);
        if (b && !prev_b) {
            usb_msc.setUnitReady(false);
            SD.begin(PIN_SD_CS);
            return;
        }
        prev_b = b;
        delay(30);
    }
}

void Bootloader::menu() {
    std::vector<String> entries;
    entries.push_back("!UPLOAD MODE!");

    std::vector<String> games = findGames(loader);
    for (const String& game : games) {
        entries.push_back(game);
    }

    size_t selected = 0;
    bool prev_up = false;
    bool prev_down = false;
    bool prev_a = false;
    bool prev_b = false;

    drawMenu(oled, entries, selected);

    while (true) {
        bool up = pressed(PIN_UP);
        bool down = pressed(PIN_DOWN);
        bool a = pressed(PIN_BTN_A);
        bool b = pressed(PIN_BTN_B);

        if (up && !prev_up && selected > 0) {
            --selected;
            drawMenu(oled, entries, selected);
        }

        if (down && !prev_down && (selected + 1) < entries.size()) {
            ++selected;
            drawMenu(oled, entries, selected);
        }

        if (a && !prev_a) {
            if (selected == 0) {
                uploadMode();
                drawMenu(oled, entries, selected);
            } else {
                launch(entries[selected].c_str());
                drawMenu(oled, entries, selected);
            }
        }

        if (b && !prev_b) {
            entries.clear();
            entries.push_back("[Upload mode USB]");
            games = findGames(loader);
            for (const String& game : games) {
                entries.push_back(game);
            }
            if (selected >= entries.size()) {
                selected = entries.size() - 1;
            }
            drawMenu(oled, entries, selected);
        }

        prev_up = up;
        prev_down = down;
        prev_a = a;
        prev_b = b;
        delay(35);
    }
}

void Bootloader::init() {

    DO_NOT_TOUCH();
    hal = create_hal();
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

    menu();
}
