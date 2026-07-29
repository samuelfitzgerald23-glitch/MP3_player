#include <SD.h>
#include "SPI_HAL.h"

File currentDir;
File currentFile;

bool isMP3 = 0;
bool isTXT = 0;

void SD_init() {

    if(!SD.begin(SD_CS_PIN, SPI)) {
        Serial.print("SD card failed");
        while(1) {};
    }

    currentDir = SD.open("/");
}