#include "sd_card.h"
#include <strings.h>
#include <cstring>

SdFs sd;

static char listNames[MAX_LIST_ITEMS][LIST_NAME_LEN];
static int  listCount = 0;

SdFs* getSD() {return &sd;}

static int cmpNames(const void* a, const void* b) {
    return strcasecmp((const char*)a, (const char*)b);
}

static bool isMp3(const char* name) {
    size_t n = strlen(name);
    return n > 4 && strcasecmp(name + n - 4, ".mp3") == 0;
}

static bool isOgg(const char* name) {
    size_t n = strlen(name);
    return n > 4 && strcasecmp(name + n - 4, ".ogg") == 0;
}

bool SD_init() {
    if (!sd.begin(SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(4)))) {
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
    mutex_enter_blocking(&busMutex);
    listCount = 0;

    FsFile dir = sd.open(path, O_RDONLY);
    if (!dir || !dir.isDir()) {
        Serial.printf("Can't open %s\n", path);
        mutex_exit(&busMutex);
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
    mutex_exit(&busMutex);
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

static inline void readAtPos(FsFile &file, uint64_t pos, uint8_t* output, size_t bytes) {
    file.seek(pos);
    file.read(output, bytes);
    output[bytes] = '\0';
    for (int i = TAGV1SIZE - 1; i >= 0 && (output[i] == '\0' || output[i] == ' '); i--) output[i] = '\0';
}

// enc: 0 = Latin-1, 1 = UTF-16 with BOM, 2 = UTF-16BE, 3 = UTF-8
// Output is ASCII only (your 4x6 font can't draw more); other characters become '?'
static void decodeText(const uint8_t* raw, size_t len, uint8_t enc, char* out, size_t outSize) {
    size_t o = 0;
    if (enc == 1 || enc == 2) {
        bool le = false;
        size_t i = 0;
        if (enc == 1 && len >= 2) {
            if (raw[0] == 0xFF && raw[1] == 0xFE)      { le = true; i = 2; }
            else if (raw[0] == 0xFE && raw[1] == 0xFF) { i = 2; }
        }
        for (; i + 1 < len && o < outSize - 1; i += 2) {
            uint8_t lo = le ? raw[i]     : raw[i + 1];
            uint8_t hi = le ? raw[i + 1] : raw[i];
            if (lo == 0 && hi == 0) break;
            out[o++] = (hi == 0 && lo >= 32 && lo < 127) ? (char)lo : '?';
        }
    } else {
        for (size_t i = 0; i < len && o < outSize - 1; i++) {
            if (raw[i] == 0) break;
            out[o++] = (raw[i] >= 32 && raw[i] < 127) ? (char)raw[i] : '?';
        }
    }
    out[o] = '\0';
}

static bool Tagv2(FsFile &file, Tag_t *outBufs, size_t bufSize = TAGARRAYSIZE) {
    file.seek(0);   //go to start of file
    uint8_t header[10];
    file.read(header, 10);

    if (memcmp(header, "ID3", 3) != 0) return 0;    //not tagv2
    outBufs->ID = MP3V2;

    uint8_t version = header[3];
    if(version < 3) return 0;

    uint32_t tagSize = readSynchsafeInt(&header[6]);
    uint32_t pos = 10;

    bool artistFilled = 0, songFilled = 0;

    while(pos < tagSize + 10) {
        uint8_t frameheader[10];
        file.seek(pos);
        if(file.read(frameheader, 10) != 10) break;

        uint32_t frameSize = (version == 4) 
        ? readSynchsafeInt(&frameheader[4]) 
        : (frameheader[4] << 24) | (frameheader[5] << 16) | (frameheader[6] << 8) | (frameheader[7]);

        if (frameSize == 0 || frameSize > tagSize) break; // sanity check, avoid junk/padding

        bool isTitle = memcmp(frameheader, TAGV2TITLE, 4) == 0;
        bool isArtist = memcmp(frameheader, TAGV2ARTIST, 4) == 0;

        if (isTitle || isArtist) {
            uint8_t raw[2 * TAGARRAYSIZE + 2];
            size_t toRead = frameSize < sizeof(raw) ? frameSize : sizeof(raw);
            int n = file.read(raw, toRead);              // already positioned after the header
            if (n > 1) {
                char* dst = isArtist ? outBufs->artist : outBufs->song;
                decodeText(raw + 1, n - 1, raw[0], dst, TAGARRAYSIZE);
                (isArtist ? artistFilled : songFilled) = 1;
            }
        }
        if(artistFilled && songFilled) {
            return 1;
        }

        pos += 10 + frameSize;
    }
    if(artistFilled || songFilled) {
            return 1;
        }

    return 0;
}

static bool Tagv1(FsFile &file, Tag_t *outBufs) {
    uint64_t size = file.size();
    if(size < 128) return 0;
    uint64_t header_start = size - 128;
    if(!file.seek(header_start)) return 0;

    char header[3];
    file.read(header, 3);
    if(memcmp(header, "TAG", 3) != 0) return 0;    //no v1 tag
    outBufs->ID = MP3V1;

    readAtPos(file, header_start + TAGV1TITLESTART, (uint8_t*)outBufs->song, TAGV1SIZE);
    outBufs->song[30] = '\0';

    readAtPos(file, header_start + TAGV1ARTISTSTART, (uint8_t*)outBufs->artist, TAGV1SIZE);
    outBufs->artist[30] = '\0';

    return 1;
}

Tag_t getMP3MetaData(const char *path) {
    mutex_enter_blocking(&busMutex);

    Tag_t data;
    int n = sizeof(data.path);
    strncpy(data.path, path, n-1);
    data.path[n-1] = '\0';

    FsFile file = sd.open(path);
    if(!file) {
        Serial.println("couldn't find fie");
        mutex_exit(&busMutex);
        return data;
    }

    if(Tagv2(file, &data) || Tagv1(file, &data)) {
        file.close(); 
        mutex_exit(&busMutex); 
        return data;
    }

    //no tags found default to file name
    const char* name = strrchr(path, '/');
    name = name ? name+1 : path;
    strncpy(data.song, name, TAGARRAYSIZE - 1);
    data.song[TAGARRAYSIZE - 1] = '\0';
    char* dot = strrchr(data.song, '.');
    if (dot) *dot = '\0';

    data.artist[0] = '\0';
    file.close();
    mutex_exit(&busMutex);
    return data;
}

void formatTag(const Tag_t& tag, char* out, size_t n) {
    if (tag.artist[0] != '\0')
        snprintf(out, n, "%s - %s", tag.song, tag.artist);
    else
        snprintf(out, n, "%s", tag.song);
}

int skipTags(FsFile& file, const audio_t audio) {
    if (audio == MP3V1) return (file.size() - 128);

    uint8_t h[10];
    if (audio == MP3V2 && file.read(h, 10) == 10) {
        uint32_t skip = 10 + readSynchsafeInt(&h[6]);
        if (h[5] & 0x10) skip += 10;     // footer present (v2.4 flag)
        file.seek(skip);
    } else {
        file.seek(0);
    }
    return file.size();
}