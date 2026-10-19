#include "OLED.h"

// Bitmap data for the images
static const unsigned char image_Attention_bits[] U8X8_PROGMEM = {0x0e,0x0a,0x0a,0x0a,0x0e,0x04,0x00,0x0e};
static const unsigned char image_back_button_bits[] U8X8_PROGMEM = {0x04,0x06,0x07,0x06,0x04};
static const unsigned char image_ButtonCenter_bits[] U8X8_PROGMEM = {0x1c,0x22,0x5d,0x5d,0x5d,0x22,0x1c};
static const unsigned char image_gear_bits[] U8X8_PROGMEM = {
    0x10,0x00,0xba,0x00,0x44,0x00,0x92,0x00,0xbb,
    0x01,0x92,0x00,0x44,0x00,0xba,0x00,0x10,0x00
};
static const unsigned char image_music_note_bits[] U8X8_PROGMEM = {0x1e,0x12,0x12,0x1b,0x1b};
static const unsigned char image_pause_bits[] U8X8_PROGMEM = {0x05,0x05,0x05,0x05,0x05};
static const unsigned char image_reccord_icon_bits[] U8X8_PROGMEM = {
    0x00,0xf0,0x07,0x00,0x00,0xfe,0x3f,0x00,0x80,0xff,0xff,0x00,0xc0,0x0f,
    0xf8,0x01,0xe0,0xf3,0xe7,0x03,0xf0,0xfc,0x9f,0x07,0x78,0xff,0x7f,0x0f,
    0xbc,0x1f,0xfc,0x1e,0xdc,0xe7,0xf3,0x1d,0xde,0xfb,0xef,0x3d,0xee,0xfd,
    0xdf,0x3b,0xee,0xfe,0xbf,0x3b,0xf7,0x3e,0xbe,0x77,0x77,0x1f,0x7c,0x77,
    0x77,0x8f,0x78,0x77,0x77,0xcf,0x79,0x77,0x77,0x8f,0x78,0x77,0x77,0x1f,
    0x7c,0x77,0xf7,0x3e,0xbe,0x77,0xee,0xfe,0xbf,0x3b,0xee,0xfd,0xdf,0x3b,
    0xde,0xfb,0xef,0x3d,0xdc,0xe7,0xf3,0x1d,0xbc,0x1f,0xfc,0x1e,0x78,0xff,
    0x7f,0x0f,0xf0,0xfc,0x9f,0x07,0xe0,0xf3,0xe7,0x03,0xc0,0x0f,0xf8,0x01,
    0x80,0xff,0xff,0x00,0x00,0xfe,0x3f,0x00,0x00,0xf0,0x07,0x00
};
static const unsigned char image_skip_button_bits[] U8X8_PROGMEM = {0x01,0x03,0x07,0x03,0x01};
static const unsigned char image_Voldwn_bits[] U8X8_PROGMEM = {0x08,0x0c,0x2f,0x2f,0x0c,0x08};
static const unsigned char image_Volup_bits[] U8X8_PROGMEM = {0x48,0x8c,0xaf,0xaf,0x8c,0x48};
static const unsigned char image_back_arrow_bits[] U8X8_PROGMEM = {0x18,0x7d,0x8f,0x07,0x0f};

U8G2_SSD1309_128X64_NONAME2_F_4W_HW_SPI u8g2(U8G2_R0, /*cs=*/ OLED_CS_PIN, /*dc=*/ OLED_DC_PIN, /*reset=*/ OLED_RESET_PIN);

OLED_Screen oled_screen(HOME_SCREEN);

OLED_Screen* getOLEDScreen() {
    return &oled_screen;
}

void setOLEDState(OLED_STATE state) {
    oled_screen.set_state(state);
}

void toggle_back(bool a) {
    oled_screen.back_button = a;
}

void OLED_Screen::draw_screen(OLED_STATE state) {
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    //draw the border
    u8g2.drawRFrame(0, 1, 126, 62, 5);

    if(oled_screen.back_button) {
        u8g2.drawXBMP(115, 55, 8, 5, image_back_arrow_bits);
    }

    switch(oled_screen.current_select) {
        case BACK:
            u8g2.drawXBMP(100, ICON_Y_ZERO + 40, 5, 8, image_Attention_bits);
            break;
        default:
            u8g2.drawXBMP(64, ICON_Y_ZERO + 10*static_cast<int>(oled_screen.current_select), 5, 8, image_Attention_bits);
            break;
    }

    switch (state) {
        case HOME_SCREEN:
            //art
            u8g2.drawXBMP(ART_X_ZERO + 7, ART_Y_ZERO + 34, 3, 5, image_back_button_bits);
            u8g2.drawXBMP(ART_X_ZERO + 21, ART_Y_ZERO + 34, 3, 5, image_skip_button_bits);
            u8g2.drawXBMP(ART_X_ZERO + 14, ART_Y_ZERO + 34, 3, 5, image_pause_bits);
            u8g2.drawXBMP(ART_X_ZERO + 31, ART_Y_ZERO - 6, 8, 6, image_Volup_bits);
            u8g2.drawXBMP(ART_X_ZERO - 8, ART_Y_ZERO - 6, 6, 6, image_Voldwn_bits);
            u8g2.drawXBMP(ART_X_ZERO, ART_Y_ZERO, 31, 31, image_reccord_icon_bits);

            //select options
            u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO, "Settings");
            u8g2.drawXBMP(ICON_X_ZERO, ICON_Y_ZERO, 9, 9, image_gear_bits);

            u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO, "Playlists");
            u8g2.drawXBMP(ICON_X_ZERO, ICON_Y_ZERO, 5, 5, image_music_note_bits);
            break;
        case SETTINGS_SCREEN:
            u8g2.drawXBMP(112, ICON_Y_ZERO, 9, 9, image_gear_bits);
            break;
        case PLAYLIST_SCREEN:
            u8g2.drawXBMP(117, 6, 5, 5, image_music_note_bits);
            break;
    }

}

void OLED_init() {
    
    u8g2.begin();
    //pinMode(CS_PIN, OUTPUT);
    
    u8g2.setPowerSave(0);
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.setFont(u8g2_font_4x6_tr);
}

void OLED_task() {
    u8g2.clearBuffer();
    u8g2.firstPage();
    do {
        oled_screen.draw_screen(oled_screen.oled_state);
    } while (u8g2.nextPage());
}