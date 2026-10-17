#include "sd_card.h"

SdFat sd;

bool SD_init() {
    if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(1))) {
        Serial.println("Failed to initialize SD card.");
        printSdError();
        return false;
    } else {
        Serial.println("SD card initialized successfully.");
        return true;
    }
}

void printSdError() {
    sd.initErrorHalt(&Serial);
}

void sd_task() {
    //does something
}