#include <SdFat.h>
#include "SPI_HAL.h"

SdFat SD;

bool isMP3 = 0;
bool isTXT = 0;

void SD_init() {
    if (!SD.begin(SD_CS_PIN, SD_SCK_MHZ(4))) {
        SD.initErrorHalt(&Serial);
    }
}

void getMetaData(const char *filename, char *titleOut, char *artistOut) {
    
}