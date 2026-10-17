#ifndef SD_CARD_H
#define SD_CARD_H

#include "SPI_HAL.h"
#include "SdFat.h"

bool SD_init();
void printSdError();

#endif // SD_CARD_H