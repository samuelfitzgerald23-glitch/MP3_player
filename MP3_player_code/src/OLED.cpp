#include "OLED.h"


// Bitmap data for the images
static const unsigned char image_Attention_bits[] U8X8_PROGMEM = {0x0e,0x0a,0x0a,0x0a,0x0e,0x04,0x00,0x0e};
static const unsigned char image_back_button_bits[] U8X8_PROGMEM = {0x04,0x06,0x07,0x06,0x04};
static const unsigned char image_ButtonCenter_bits[] U8X8_PROGMEM = {0x1c,0x22,0x5d,0x5d,0x5d,0x22,0x1c};
static const unsigned char image_gear_bits[] U8X8_PROGMEM = {0x10,0x00,0xba,0x00,0x44,0x00,0x92,0x00,0xbb,0x01,0x92,0x00,0x44,0x00,0xba,0x00,0x10,0x00};
static const unsigned char image_music_note_bits[] U8X8_PROGMEM = {0x1e,0x12,0x12,0x1b,0x1b};
static const unsigned char image_pause_bits[] U8X8_PROGMEM = {0x05,0x05,0x05,0x05,0x05};
static const unsigned char image_reccord_icon_bits[] U8X8_PROGMEM = {0x00,0xf0,0x07,0x00,0x00,0xfe,0x3f,0x00,0x80,0xff,0xff,0x00,0xc0,0x0f,0xf8,0x01,0xe0,0xf3,0xe7,0x03,0xf0,0xfc,0x9f,0x07,0x78,0xff,0x7f,0x0f,0xbc,0x1f,0xfc,0x1e,0xdc,0xe7,0xf3,0x1d,0xde,0xfb,0xef,0x3d,0xee,0xfd,0xdf,0x3b,0xee,0xfe,0xbf,0x3b,0xf7,0x3e,0xbe,0x77,0x77,0x1f,0x7c,0x77,0x77,0x8f,0x78,0x77,0x77,0xcf,0x79,0x77,0x77,0x8f,0x78,0x77,0x77,0x1f,0x7c,0x77,0xf7,0x3e,0xbe,0x77,0xee,0xfe,0xbf,0x3b,0xee,0xfd,0xdf,0x3b,0xde,0xfb,0xef,0x3d,0xdc,0xe7,0xf3,0x1d,0xbc,0x1f,0xfc,0x1e,0x78,0xff,0x7f,0x0f,0xf0,0xfc,0x9f,0x07,0xe0,0xf3,0xe7,0x03,0xc0,0x0f,0xf8,0x01,0x80,0xff,0xff,0x00,0x00,0xfe,0x3f,0x00,0x00,0xf0,0x07,0x00};
static const unsigned char image_skip_button_bits[] U8X8_PROGMEM = {0x01,0x03,0x07,0x03,0x01};
static const unsigned char image_Voldwn_bits[] U8X8_PROGMEM = {0x08,0x0c,0x2f,0x2f,0x0c,0x08};
static const unsigned char image_Volup_bits[] U8X8_PROGMEM = {0x48,0x8c,0xaf,0xaf,0x8c,0x48};
static const unsigned char image_back_arrow_bits[] U8X8_PROGMEM = {0x18,0x7d,0x8f,0x07,0x0f};

static int Attention_y = ICON_Y_ZERO;
static int Attention_x = 64;

const char* song_name_text = "Song name - Aritist";

U8G2_SSD1309_128X64_NONAME2_F_4W_SW_SPI u8g2(U8G2_R0, SCL_PIN, SDA_PIN, CS_PIN, DC_PIN, RESET_PIN);

//children ID arrays hold the ID of the children screens for each screen
static uint8_t mainChildren[2] = {1, 1};
static uint8_t settingsChildren[4] = {1, 1, 1, 1};

OLED_Screen_t OLED_Screens[] = {
    {0, 2, OLED_MainScreen, SELECT0, {1, 1}, 0},
    {1, 4, OLED_SettingsScreen, SELECT0, {1, 1, 1, 1}, 0}
};

static OLED_Screen_t* currentScreen = &OLED_Screens[0];

OLED_Screen_t* getCurrentScreen() {
    return currentScreen;
}

void changeScreen() {
    if (currentScreen->currentSelect == BACK) {
        currentScreen = &OLED_Screens[currentScreen->parentID];
    } else {
        currentScreen = &OLED_Screens[currentScreen->childrenIDs[currentScreen->currentSelect]];
    }
    currentScreen->currentSelect = SELECT0;
}

void OLED_init() {
    //I dont know why but without this the cs pin is not set to low and the oled does not work
    pinMode(CS_PIN, OUTPUT);
    
    u8g2.begin();
    
    u8g2.setPowerSave(0);
    //u8g2.setContrast(255);
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.setFont(u8g2_font_4x6_tr);
}

void OLED_MainScreen() {
    // Icons
    u8g2.drawXBMP(ICON_X_ZERO - 1, ICON_Y_ZERO, 9, 9, image_gear_bits);
    u8g2.drawXBMP(ICON_X_ZERO, ICON_Y_ZERO + 10, 7, 7, image_ButtonCenter_bits);
    // Playback icons
    u8g2.drawXBMP(PLAYBACK_X_ZERO + 7, PLAYBACK_Y_ZERO + 34, 3, 5, image_back_button_bits);
    u8g2.drawXBMP(PLAYBACK_X_ZERO + 21, PLAYBACK_Y_ZERO + 34, 3, 5, image_skip_button_bits);
    u8g2.drawXBMP(PLAYBACK_X_ZERO + 14, PLAYBACK_Y_ZERO + 34, 3, 5, image_pause_bits);
    u8g2.drawXBMP(PLAYBACK_X_ZERO + 31, PLAYBACK_Y_ZERO - 6, 8, 6, image_Volup_bits);
    u8g2.drawXBMP(PLAYBACK_X_ZERO - 8, PLAYBACK_Y_ZERO - 6, 6, 6, image_Voldwn_bits);
    u8g2.drawXBMP(PLAYBACK_X_ZERO, PLAYBACK_Y_ZERO, 31, 31, image_reccord_icon_bits);
    // text
    u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO, "Settings");
    u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO + 10, "Playlists");
}
void OLED_SettingsScreen() {
    // Icons
    u8g2.drawXBMP(ICON_X_ZERO, ICON_Y_ZERO, 7, 7, image_ButtonCenter_bits);
    u8g2.drawXBMP(ICON_X_ZERO, ICON_Y_ZERO + 10, 7, 7, image_ButtonCenter_bits);
    u8g2.drawXBMP(ICON_X_ZERO, ICON_Y_ZERO + 20, 7, 7, image_ButtonCenter_bits);
    u8g2.drawXBMP(ICON_X_ZERO, ICON_Y_ZERO + 30, 7, 7, image_ButtonCenter_bits);
    // Text
    u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO, "Edit songs");
    u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO + 10, "Playback");
    u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO + 20, "Equaliser");
    u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO + 30, "Sleep");
}

void OLEDTask() {
    Attention_x = 64;
    switch (currentScreen->currentSelect) {
        case SELECT0:
            Attention_y = ICON_Y_ZERO;
            break;
        case SELECT1:
            Attention_y = ICON_Y_ZERO + 10;
            break;
        case SELECT2:
            Attention_y = ICON_Y_ZERO + 20;
            break;
        case SELECT3:
            Attention_y = ICON_Y_ZERO + 30;
            break;
        case SELECT4:
            Attention_y = ICON_Y_ZERO + 40;
            break;
        case BACK:
            Attention_y = ICON_Y_ZERO + 40;
            Attention_x = 115;
            break;
    }

    digitalWrite(CS_PIN, LOW);
    u8g2.firstPage();
    do {
        u8g2.setFontMode(1);
        u8g2.setBitmapMode(1);
        u8g2.drawRFrame(0, 1, 126, 62, 5);
        currentScreen->drawScreen();
        //select identifier
        u8g2.drawXBMP(Attention_x, Attention_y, 5, 8, image_Attention_bits);
        // song name
        u8g2.drawXBMP(ICON_X_ZERO + 1, 55, 5, 5, image_music_note_bits);
        u8g2.drawStr(TEXT_X_ZERO, 60, song_name_text);
        if(currentScreen->id != 0) {
            //only draw back arrow if not the main screen
            u8g2.drawXBMP(115, 55, 8, 5, image_back_arrow_bits);
        }
    } while (u8g2.nextPage());
    digitalWrite(CS_PIN, HIGH);
}