#ifndef SD_CARD_H
#define SD_CARD_H

#include "SPI_HAL.h"
#include "SdFat.h"

#define MAX_LIST_ITEMS  128
#define LIST_NAME_LEN   64

#define TAGV2ARTIST "TPE1"
#define TAGV2TITLE "TIT2"

#define TAGV1TITLESTART 3
#define TAGV1ARTISTSTART 33
#define TAGV1SIZE 30

#define TAGARRAYSIZE 64

enum ListMode { LIST_FOLDERS, LIST_MP3 };

struct Tag_t {
    int ID = 0;         //0 = none, 1 = v1, 2 = v2
    char song[TAGARRAYSIZE] = {0};
    char artist[TAGARRAYSIZE] = {0};
};

int         loadList(const char* path, ListMode mode);
int         getListCount();
const char* getListName(int i);

Tag_t getMP3MetaData(const char *path);
void formatTag(const Tag_t& tag, char* out, size_t n);

bool SD_init();
void printSdError();
void clearCashe();

#endif // SD_CARD_H