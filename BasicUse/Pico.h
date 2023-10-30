#ifndef TLC_PICO_H
#define TLC_PICO_H

#if DATA_TRANSFER_MODE == TLC_BITBANG
#error "If you want bitbang mode, insert pin defs here"
#endif

// MOSI (Teensy++ pin 22) -> SIN (TLC pin 26)
#define TLC_MOSI_PIN	3
#define TLC_MOSI_PORT	0
#define TLC_MOSI_DDR	0

// SCK (Teensy++ pin 21) -> SCLK (TLC pin 25)
#define TLC_SCK_PIN		2
#define TLC_SCK_PORT	0
#define TLC_SCK_DDR		0

// SS (Teensy++ pin 20)
#define TLC_SS_PIN		4
#define TLC_SS_DDR		0

// OC1A (Teensy++ pin 25) -> XLAT (TLC pin 24)
#define XLAT_PIN		5
#define XLAT_PORT		0
#define XLAT_DDR		0

// OC1B (Teensy++ pin 26) -> BLANK (TLC pin 23)
#define BLANK_PIN		6
#define BLANK_PORT		0
#define BLANK_DDR		0

// OC2B (Teensy++ pin 1) -> GSCLK (TLC pin 18)
#define GSCLK_PIN		7
#define GSCLK_PORT		0
#define GSCLK_DDR		0

#endif

