
// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI0_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19


#include "hw_config.h"

// Hardware Configuration of SPI "objects"
// Note: multiple SD cards can be driven by one SPI if they use different slave select
static spi_t spi = { // One for each SPI.
  
  .hw_inst = SPI0_PORT, // SPI component
  .miso_gpio = PIN_MISO,  // GPIO number (not Pico pin number)
  .mosi_gpio = PIN_MOSI,
  .sck_gpio = PIN_SCK,

    //.baud_rate = 125 * 1000 * 1000 / 8  // 15625000 Hz
    //.baud_rate = 125 * 1000 * 1000 / 6  // 20833333 Hz
    .baud_rate = 125 * 1000 * 1000 / 4  // 31250000 Hz
    //.baud_rate = 125 * 1000 * 1000 / 2  // 62500000 Hz
};
/* SPI Interface */
static sd_spi_if_t spi_if = {
    .spi = &spi,  // Pointer to the SPI driving this card
    .ss_gpio = PIN_CS  // The SPI slave select GPIO for this SD card
};
/* Configuration of the SD Card socket object */
static sd_card_t sd_card = {
    .type = SD_IF_SPI,
    .spi_if_p = &spi_if  // Pointer to the SPI interface driving this card
};



size_t sd_get_num() { return 1; }

/**
 * @brief Get a pointer to an SD card object by its number.
 *
 * @param[in] num The number of the SD card to get.
 *
 * @return A pointer to the SD card object, or @c NULL if the number is invalid.
 */
sd_card_t *sd_get_by_num(size_t num) {
    if (0 == num) {
        // The number 0 is a valid SD card number.
        // Return a pointer to the sd_card object.
        return &sd_card;
    } else {
        // The number is invalid. Return @c NULL.
        return NULL;
    }
}