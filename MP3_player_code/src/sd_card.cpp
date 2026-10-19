#include "sd_card.h"
#include <strings.h>

static char listNames[MAX_LIST_ITEMS][LIST_NAME_LEN];
static int  listCount = 0;

static int cmpNames(const void* a, const void* b) {
    return strcasecmp((const char*)a, (const char*)b);
}

static bool isMp3(const char* name) {
    size_t n = strlen(name);
    return n > 4 && strcasecmp(name + n - 4, ".mp3") == 0;
}

SdFs sd;

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

int loadList(const char* path, ListMode mode) {
    listCount = 0;

    FsFile dir = sd.open(path, O_RDONLY);
    if (!dir || !dir.isDir()) {
        Serial.printf("Can't open %s\n", path);
        return 0;
    }

    FsFile entry;
    while (listCount < MAX_LIST_ITEMS && entry.openNext(&dir, O_RDONLY)) {
        char* slot = listNames[listCount];
        entry.getName(slot, LIST_NAME_LEN);

        bool keep = slot[0] != '.' && !entry.isHidden() &&
                    (mode == LIST_FOLDERS ? entry.isDir()
                                          : (entry.isFile() && isMp3(slot)));
        if (keep) listCount++;
        entry.close();
    }
    dir.close();

    qsort(listNames, listCount, LIST_NAME_LEN, cmpNames);
    return listCount;
}

int getListCount() { return listCount; }

const char* getListName(int i) {
    return (i >= 0 && i < listCount) ? listNames[i] : "";
}