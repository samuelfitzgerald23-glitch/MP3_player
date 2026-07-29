#ifndef SPI_HAL_H
#define SPI_HAL_H

#include <SPI.h>

#define SCL_PIN 2
#define TX_PIN 3    //MOSI
#define RX_PIN 4    //MISO

//for OLED screen
#define OLED_RESET_PIN 6
#define OLED_DC_PIN 7
#define OLED_CS_PIN 8

//for SD card
#define SD_CS_PIN 9

//for audio decoder
#define AUDIO_RESET_PIN
#define AUDIO_CS_PIN

void SPI_init();

#endif //SPI_HAL_H