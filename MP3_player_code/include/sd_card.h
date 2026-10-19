#ifndef SD_CARD_H
#define SD_CARD_H

#include "SPI_HAL.h"
#include "SdFat.h"

#define MAX_LIST_ITEMS  128
#define LIST_NAME_LEN   64

enum ListMode { LIST_FOLDERS, LIST_MP3 };

int         loadList(const char* path, ListMode mode);
int         getListCount();
const char* getListName(int i);

bool SD_init();
void printSdError();

#endif // SD_CARD_H