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

static const char* song_name_text = "Song name - Artist";

static bool enableScroll = 0;

static uint8_t brightness = 120;

U8G2_SSD1309_128X64_NONAME2_F_4W_HW_SPI u8g2(U8G2_R0, /*cs=*/ OLED_CS_PIN, /*dc=*/ OLED_DC_PIN, /*reset=*/ OLED_RESET_PIN);

static screen_t screen = {0, 0, SELECT0, 0, 0};

/**********************************************************/
//getters

screen_t* getScreen() {
    return &screen;
}

int getRuntimeSelects() {
    return screen.runtimeSelects;
}

/**********************************************************/
//screen manipulation functions

void setEnableScroll(bool a) {
    enableScroll = a;
}

void changeBrightness(int a) {
    brightness = std::clamp((brightness + 17*a), 0, 255);
}

void resetScreen(state_t* state) {
    int n = state->numChildren;
    screen.currentSelect = SELECT0;
    screen.page = 0;
    screen.pageCount = (n + ITEMS_PER_PAGE - 1)/ITEMS_PER_PAGE;
    screen.N_selects = n;
    screen.runtimeSelects = n;
}

void scrollScreen() {
    screen.page++;
    screen.runtimeSelects -= ITEMS_PER_PAGE;
    if (screen.page >= screen.pageCount) {
        screen.page = 0;
        screen.runtimeSelects = getState()->numChildren;
    }
}

/*************************************************************************/
//screen drawing functions

void DrawStateOptions(state_t* state) {
    int item_offset = screen.page*ITEMS_PER_PAGE;
    for (int i = 0; i < ITEMS_PER_PAGE; i++) {
        if ((i + item_offset) < state->numChildren) {
            state_t* child = state->childrenStates[i];
            u8g2.drawXBMP(ICON_X_ZERO, ICON_Y_ZERO + (i * 10), 7, 7, image_ButtonCenter_bits);
            u8g2.drawStr(TEXT_X_ZERO, TEXT_Y_ZERO + (i * 10), child->displayName);
        }
    }
}

void drawScreen(state_t* state) {
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);

    //draw the frame
    u8g2.drawRFrame(0, 1, 126, 62, 5);

    //select identifier
    u8g2.drawXBMP(Attention_x, Attention_y, 5, 8, image_Attention_bits);

    //song name
    u8g2.drawXBMP(ICON_X_ZERO + 1, 55, 5, 5, image_music_note_bits);
    u8g2.drawStr(TEXT_X_ZERO, 60, song_name_text);

    //only draw back arrow if not on the start screen
    if(state->name != START) {
        u8g2.drawXBMP(115, 55, 8, 5, image_back_arrow_bits);
        u8g2.drawStr(78, TEXT_Y_ZERO, state->displayName);
    }

    if(state->childrenStates[0] != nullptr) DrawStateOptions(state);


    switch (state->name) {
        case START:
            //draw start screen stuff

            u8g2.drawXBMP(ART_X_ZERO + 7, ART_Y_ZERO + 34, 3, 5, image_back_button_bits);
            u8g2.drawXBMP(ART_X_ZERO + 21, ART_Y_ZERO + 34, 3, 5, image_skip_button_bits);
            u8g2.drawXBMP(ART_X_ZERO + 14, ART_Y_ZERO + 34, 3, 5, image_pause_bits);
            u8g2.drawXBMP(ART_X_ZERO + 31, ART_Y_ZERO - 6, 8, 6, image_Volup_bits);
            u8g2.drawXBMP(ART_X_ZERO - 8, ART_Y_ZERO - 6, 6, 6, image_Voldwn_bits);
            u8g2.drawXBMP(ART_X_ZERO, ART_Y_ZERO, 31, 31, image_reccord_icon_bits);
            break;
        case SETTINGS:
            //draw setting screen stuff

            u8g2.drawXBMP(112, ICON_Y_ZERO, 9, 9, image_gear_bits);            
            break;
        case PLAYLISTS:
            //draw playlists

            u8g2.drawXBMP(117, 6, 5, 5, image_music_note_bits);
            screen.currentSelect = BACK;
            break;
        case SONGS:
            //draw songs in selected playlist

            u8g2.drawXBMP(117, 6, 5, 5, image_music_note_bits);
            screen.currentSelect = BACK;
            break;
        case EQUALISER:
            //draw equaliser slider
            u8g2.drawLine(2, 32, 125, 32);
            u8g2.drawBox(60, 30, 8, 5);
            screen.currentSelect = BACK;
            break;
        case PLAYBACK:
            //draw playback options
            screen.currentSelect = BACK;
            break;
        case BRIGHTNESS:
            //draw brightness slider
            u8g2.drawLine(2, 32, 125, 32);
            u8g2.drawBox(map(brightness, 0, 255, 4, 118), 30, 8, 5);

            u8g2.setCursor(TEXT_X_ZERO, TEXT_Y_ZERO);
            u8g2.print("Brightness:");
            u8g2.print(brightness);
            screen.currentSelect = BACK;
            break;
        default:
            break;
    }
}

/**************************************************************************/
//task funcs used by main

void OLED_init() {
    resetScreen(getState());
    
    u8g2.begin();
    //pinMode(CS_PIN, OUTPUT);
    
    u8g2.setPowerSave(0);
    u8g2.clearBuffer();
    u8g2.setFontMode(1);
    u8g2.setBitmapMode(1);
    u8g2.setFont(u8g2_font_4x6_tr);
}

void OLEDTask() {
    Attention_x = 64;
    switch (screen.currentSelect) {
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
            Attention_x = 115;
            Attention_y = ICON_Y_ZERO + 40;
    }

    if (enableScroll) {
        enableScroll = 0;
        scrollScreen();
    }

    u8g2.setContrast(brightness);

    //set gpio 6 (cs) with port manipulation
    //sio_hw->gpio_togl = 1u << OLED_CS_PIN;

    u8g2.firstPage();
    do {
        drawScreen(getState());
    } while (u8g2.nextPage());

    //sio_hw->gpio_togl = 1u << OLED_CS_PIN;
}