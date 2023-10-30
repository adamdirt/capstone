#define VPRG_ENABLED 0
#define XERR_ENABLED 0

#include "Pico.h"


void TLCinit(uint16_t initialValue)
{
    /* Pin Setup */
    pinMode(XLAT_PIN, OUTPUT);
    output_pin(BLANK_DDR, BLANK_PIN);
    output_pin(GSCLK_DDR, GSCLK_PIN);
#if VPRG_ENABLED
    output_pin(VPRG_DDR, VPRG_PIN);
    clear_pin(VPRG_PORT, VPRG_PIN);  // grayscale mode (VPRG low)
#endif
#if XERR_ENABLED
    pullup_pin(XERR_DDR, XERR_PORT, XERR_PIN); // XERR as input, enable pull-up resistor
#endif
    set_pin(BLANK_PORT, BLANK_PIN); // leave blank high (until the timers start)

    tlc_shift8_init();

    setAll(initialValue);
    update();
    disable_XLAT_pulses();
    clear_XLAT_interrupt();
    tlc_needXLAT = 0;
    pulse_pin(XLAT_PORT, XLAT_PIN);


    /* Timer Setup */
#if defined(__AVR__)
    /* Timer 1 - BLANK / XLAT */
    TCCR1A = _BV(COM1B1);  // non inverting, output on OC1B, BLANK
    TCCR1B = _BV(WGM13);   // Phase/freq correct PWM, ICR1 top
    OCR1A = 1;             // duty factor on OC1A, XLAT is inside BLANK
    OCR1B = 2;             // duty factor on BLANK (larger than OCR1A (XLAT))
    ICR1 = TLC_PWM_PERIOD; // see tlc_config.h

    /* Timer 2 - GSCLK */
#if defined(TLC_ATMEGA_8_H)
    TCCR2  = _BV(COM20)       // set on BOTTOM, clear on OCR2A (non-inverting),
           | _BV(WGM21);      // output on OC2B, CTC mode with OCR2 top
    OCR2   = TLC_GSCLK_PERIOD / 2; // see tlc_config.h
    TCCR2 |= _BV(CS20);       // no prescale, (start pwm output)
#elif defined(TLC_TIMER3_GSCLK)
    TCCR3A = _BV(COM3A1)      // set on BOTTOM, clear on OCR3A (non-inverting),
                              // output on OC3A
           | _BV(WGM31);      // Fast pwm with ICR3 top
    OCR3A = 0;                // duty factor (as short a pulse as possible)
    ICR3 = TLC_GSCLK_PERIOD;  // see tlc_config.h
    TCCR3B = _BV(CS30)        // no prescale, (start pwm output)
           | _BV(WGM32)       // Fast pwm with ICR3 top
           | _BV(WGM33);      // Fast pwm with ICR3 top
#else
    TCCR2A = _BV(COM2B1)      // set on BOTTOM, clear on OCR2A (non-inverting),
                              // output on OC2B
           | _BV(WGM21)       // Fast pwm with OCR2A top
           | _BV(WGM20);      // Fast pwm with OCR2A top
    TCCR2B = _BV(WGM22);      // Fast pwm with OCR2A top
    OCR2B = 0;                // duty factor (as short a pulse as possible)
    OCR2A = TLC_GSCLK_PERIOD; // see tlc_config.h
    TCCR2B |= _BV(CS20);      // no prescale, (start pwm output)
#endif
    TCCR1B |= _BV(CS10);      // no prescale, (start pwm output)

#elif defined(__arm__) && defined(TEENSYDUINO)
 #if defined(__IMXRT1062__)
    /* Teensy 4.0, 4.1, MicroMod */
    clear_pin(XLAT_DDR, XLAT_PIN);
    analogWriteFrequency(5, 4000000);
    analogWrite(5, 128);
    const uint32_t newdiv = (uint32_t)((float)F_BUS_ACTUAL / 4 / 1000 + 0.5f);
    FLEXPWM4_MCTRL |= FLEXPWM_MCTRL_CLDOK(4);
    FLEXPWM4_SM2CTRL = FLEXPWM_SMCTRL_FULL | FLEXPWM_SMCTRL_PRSC(2); // 3146
    FLEXPWM4_SM2VAL0 = newdiv - 1;
    FLEXPWM4_SM2VAL1 = newdiv - 1;
    FLEXPWM4_SM2VAL2 = newdiv - 7; // pin 2 = FlexPWM4_2_A = BLANK
    FLEXPWM4_SM2VAL3 = newdiv - 1;
    FLEXPWM4_SM2VAL4 = newdiv - 6; // pin 3 = FlexPWM4_2_B = XLAT
    FLEXPWM4_SM2VAL5 = newdiv - 2;
    FLEXPWM4_OUTEN = FLEXPWM_OUTEN_PWMA_EN(4) | FLEXPWM_OUTEN_PWMB_EN(4);
    FLEXPWM4_MCTRL |= FLEXPWM_MCTRL_LDOK(4);
    CORE_PIN2_CONFIG = 1;
    CORE_PIN2_PADCONFIG = IOMUXC_PAD_DSE(7);
    CORE_PIN3_PADCONFIG = IOMUXC_PAD_DSE(7);
    FLEXPWM4_SM2INTEN = 0;
    FLEXPWM4_SM2STS = 0x3FFF;
    attachInterruptVector(IRQ_FLEXPWM4_2, flexpwm42_isr);
    NVIC_ENABLE_IRQ(IRQ_FLEXPWM4_2);
 #else
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
 #endif
#endif
    update();
}