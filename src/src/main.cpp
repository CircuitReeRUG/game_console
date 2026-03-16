#include "loader/boot.h"
#include "shared.h"


int main() {
    Bootloader bootloader;
    bootloader.init();
    
    return 0;
}
