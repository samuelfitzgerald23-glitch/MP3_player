#include "sd_card.h"
#include <strings.h>
#include <cstring>

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

void clearCashe() {
    std::memset(listNames, 0, sizeof(listNames));
}

static uint32_t readSynchsafeInt(uint8_t *bytes) {
  return ((bytes[0] & 0x7F) << 21) | ((bytes[1] & 0x7F) << 14) |
         ((bytes[2] & 0x7F) << 7)  | (bytes[3] & 0x7F);
}

static bool Tagv2(FsFile &file, Tag_t *outBufs, size_t bufSize = 64) {
    file.seek(0);   //go to start of file
    uint8_t header[10];
    file.read(header, 10);

    if (strncmp((char*)header, "ID3", 3) != 0) return 0;    //not tagv2

    uint32_t tagSize = readSynchsafeInt(&header[6]);
    uint32_t pos = 10;

    bool artistFilled = 0, songFilled = 0;

    while(pos < tagSize + 10) {
        uint8_t frameheader[10];
        file.seek(pos);
        if(file.read(frameheader, 10) != 10) break;

        uint32_t frameSize = (frameheader[4] << 24) | (frameheader[5] << 16) | (frameheader[6] << 8) | (frameheader[7]);
        if (frameSize == 0 || frameSize > tagSize) break; // sanity check, avoid junk/padding

        if(strncmp((char*)frameheader, (char*)TAGV2ARTIST, 4) == 0) {
            file.seek(pos+10+1);
            size_t toRead = min(frameSize-1, bufSize-1);
            file.read((uint8_t*)outBufs->artist, toRead);
            outBufs->artist[toRead] = '\0';
        } else if(strncmp((char*)frameheader, (char*)TAGV2SONG_NAME, 4) == 0) {
            file.seek(pos+10+1);
            size_t toRead = min(frameSize-1, bufSize-1);
            file.read((uint8_t*)outBufs->song, toRead);
            outBufs->song[toRead] = '\0';
        }
        if(artistFilled && songFilled) return 1;
    }
    return 0;
}

static bool Tagv1(FsFile &file, Tag_t *outBufs) {

}

Tag_t* getMP3MetaData(const char *path) {
    Tag_t* data;

    FsFile file = sd.open(path);

    if(Tagv2(file, data)) return data;
}