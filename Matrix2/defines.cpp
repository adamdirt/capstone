#include "defines.h"

// CRGBPalette16 myPalette = RainbowStripeColors_p;

//Set LEDX from the I2C to a certain color
void SetLEDX(uint8_t* buffer){
  uint8_t row = buffer[0];
  uint8_t column = buffer[1];
  uint8_t ledIndex = XY(column, row);
  //If trying to update led corner don't update leds
  if (ledIndex == 0 || ledIndex == 1 || ledIndex == XY(0, 1) || ledIndex == XY(1, 1)) return;

  //Else update LEDs
  leds[ledIndex].setRGB(buffer[2], buffer[3], buffer[4]);
  FastLED.show();
}

//Self commenting code right here boss
void InitializeIndicatorLight(CRGB color){
  leds[0] = color;
  leds[1] = CRGB::White;
  leds[XY(0,1)] = CRGB::White;
  leds[XY(1,1)] = CRGB::White;
  FastLED.show();
}

//Play snake animation
void Snake(void){
  for(int row = 0; row < ROWS; row++){
    for(int col = 0; col < COLUMNS; col++){
      uint16_t index = XY(col, row);
      leds[index] = CRGB::Red;
      FastLED.show();
      leds[index] = CRGB::Black;
      delay(50);
    }
  }
}

//(Column, Row) -> Index
uint16_t XY(uint8_t x, uint8_t y){
  uint16_t i;
  if( kMatrixSerpentineLayout == false) {
    if (kMatrixVertical == false) {
      i = (y * COLUMNS) + x;
    } else {
      i = ROWS * (COLUMNS - (x+1))+y;
    }
  }

  if( kMatrixSerpentineLayout == true) {
    if (kMatrixVertical == false) {
      if( y & 0x01) {
        // Odd rows run backwards
        uint8_t reverseX = (COLUMNS - 1) - x;
        i = (y * COLUMNS) + reverseX;
      } else {
        // Even rows run forwards
        i = (y * COLUMNS) + x;
      }
    } else { // vertical positioning
      if ( x & 0x01) {
        i = ROWS * (COLUMNS - (x+1))+y;
      } else {
        i = ROWS * (COLUMNS - x) - (y+1);
      }
    }
  }
  
  return i;
}
