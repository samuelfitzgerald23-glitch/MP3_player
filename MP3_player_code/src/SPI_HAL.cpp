#include "SPI_HAL.h"

void SPI_init() {
    //configure pin peripherals
    
    //OLED screen
    pinMode(OLED_CS_PIN, OUTPUT);
    digitalWrite(OLED_CS_PIN, 1);    // idle high before anything talks to the bus
    pinMode(OLED_DC_PIN, OUTPUT);
    pinMode(OLED_RESET_PIN, OUTPUT);

    //SD card
    pinMode(SD_CS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, 1);

    SPI.setSCK(SCL_PIN);
    SPI.setTX(TX_PIN);
    SPI.setRX(RX_PIN);
    SPI.begin();
}