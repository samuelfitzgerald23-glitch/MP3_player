#ifndef SD_CARD_H
#define SD_CARD_H

#include "SPI_HAL.h"
#include "SdFat.h"

#define MAX_LIST_ITEMS  128
#define LIST_NAME_LEN   64

#define TAGV2ARTIST 'TPE1'
#define TAGV2SONG_NAME 'TIT2'

enum ListMode { LIST_FOLDERS, LIST_MP3 };

struct Tag_t {
    char song[64];
    char artist[64];
};

int         loadList(const char* path, ListMode mode);
int         getListCount();
const char* getListName(int i);

bool SD_init();
void printSdError();
void clearCashe();

#endif // SD_CARD_H