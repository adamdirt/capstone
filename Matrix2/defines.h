#ifndef defines_h
#define defines_h

#include <FastLED.h>
#include <Wire.h>
#include <Arduino.h>

//Macros
#define DATA_PIN 2
#define COLUMNS 25
#define ROWS 20
#define NUM_LEDS (COLUMNS * ROWS)
#define TIMER_FREQ_HZ 500UL

//Variables
extern CRGB leds[NUM_LEDS];

//DONT TOUCH
const bool kMatrixSerpentineLayout = true;
const bool kMatrixVertical = false;

//Functions
void SetLEDX(uint8_t* buffer);
void InitializeIndicatorLight(CRGB color);
uint16_t XY(uint8_t x, uint8_t y);
void Snake(void);

#endif   