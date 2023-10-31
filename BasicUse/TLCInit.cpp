#include "Pico.h"
#include "pin_functions.h"

#define VPRG_ENABLED 0
#define XERR_ENABLED 0
#define NUM_TLCS 1

uint8_t tlc_GSData[NUM_TLCS * 24];
volatile uint8_t tlc_needXLAT;
static uint8_t firstGSInput;

uint8_t update(void)
{
    if (tlc_needXLAT) {
        return 1;
    }
    disable_XLAT_pulses();
    if (firstGSInput) {
        // adds an extra SCLK pulse unless we've just set dot-correction data
        firstGSInput = 0;
    } else {
        pulse_pin(SCLK_PIN);
    }
    uint8_t *p = tlc_GSData;
    while (p < tlc_GSData + NUM_TLCS * 24) {
        tlc_shift8(*p++);
        tlc_shift8(*p++);
        tlc_shift8(*p++);
    }
    tlc_needXLAT = 1;
    enable_XLAT_pulses();
    set_XLAT_interrupt();
    return 0;
}

void setAll(uint16_t value)
{
    uint8_t firstByte = value >> 4;
    uint8_t secondByte = (value << 4) | (value >> 8);
    uint8_t *p = tlc_GSData;
    while (p < tlc_GSData + NUM_TLCS * 24) {
        *p++ = firstByte;
        *p++ = secondByte;
        *p++ = (uint8_t)value;
    }
}

void tlc_shift8_init(void)
{
    output_pin(SIN_PIN);       // SPI MOSI as output
    output_pin(SCLK_PIN);     // SPI SCK as output
    output_pin(TLC_SS_PIN); // SPI SS as output

    clear_pin(SCLK_PIN);

    SPI(SIN_PIN, NC, SCLK_PIN);
}

void TLCinit(uint16_t initialValue)
{
    /* Pin Setup */
    pinMode(XLAT_PIN, OUTPUT);
    output_pin(BLANK_PIN);
    output_pin(GSCLK_PIN);
#if VPRG_ENABLED
    output_pin(VPRG_PIN);
    clear_pin(VPRG_PIN);  // grayscale mode (VPRG low)
#endif
#if XERR_ENABLED
    pullup_pin(XERR_PIN); // XERR as input, enable pull-up resistor
#endif
    set_pin(BLANK_PIN); // leave blank high (until the timers start)

    tlc_shift8_init();

    setAll(initialValue);
    update();
    disable_XLAT_pulses();
    clear_XLAT_interrupt();
    tlc_needXLAT = 0;
    pulse_pin(XLAT_PORT, XLAT_PIN);


    /* Timer Setup */
    TCCR2A = _BV(COM2B1)      // set on BOTTOM, clear on OCR2A (non-inverting),
                              // output on OC2B
           | _BV(WGM21)       // Fast pwm with OCR2A top
           | _BV(WGM20);      // Fast pwm with OCR2A top
    TCCR2B = _BV(WGM22);      // Fast pwm with OCR2A top
    OCR2B = 0;                // duty factor (as short a pulse as possible)
    OCR2A = TLC_GSCLK_PERIOD; // see tlc_config.h
    TCCR2B |= _BV(CS20);      // no prescale, (start pwm output)
    TCCR1B |= _BV(CS10);      // no prescale, (start pwm output)

    /* Teensy 3.0, 3.1, 3.2, 3.5, 3.6 */
    clear_pin(XLAT_DDR, XLAT_PIN);
    SIM_SCGC4 |= SIM_SCGC4_CMT;
    CMT_MSC = 0;
    CMT_PPS = 0;
    CMT_CGH1 = TLC_TIMER_TEENSY3_NORMAL_CGH1;
    CMT_CGL1 = TLC_TIMER_TEENSY3_NORMAL_CGL1;
    CMT_CMD1 = 1;
    CMT_CMD2 = 0;
    CMT_CMD3 = 0;
    CMT_CMD4 = 0;
    CMT_OC = 0x60;
    CMT_MSC = 0x01;
    CORE_PIN5_CONFIG = PORT_PCR_MUX(2)|PORT_PCR_DSE|PORT_PCR_SRE;
    FTM1_SC = 0;
    FTM1_MOD = TLC_TIMER_TEENSY3_NORMAL_MOD;
    FTM1_CNT = 0;
    FTM1_C0SC = 0x24;
    FTM1_C1SC = 0x24;
    FTM1_C0V = TLC_TIMER_TEENSY3_NORMAL_MOD - TLC_TIMER_TEENSY3_NORMAL_CV;
    FTM1_C1V = TLC_TIMER_TEENSY3_NORMAL_MOD - TLC_TIMER_TEENSY3_NORMAL_CV - 1;
    FTM1_SC = FTM_SC_CLKS(1) | FTM_SC_CPWMS;
    NVIC_ENABLE_IRQ(IRQ_FTM1);
    CORE_PIN4_CONFIG = PORT_PCR_MUX(3)|PORT_PCR_DSE|PORT_PCR_SRE;

    update();
}