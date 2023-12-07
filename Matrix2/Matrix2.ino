#include "defines.h"

#define RUN_I2C true

CRGB leds[NUM_LEDS];
CRGB currentColor;

void setup(){
  //Intialize LEDs
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear(true);
  FastLED.setMaxPowerInVoltsAndMilliamps(5,300);
  InitializeIndicatorLight(CRGB::Black);

  //Initialize console and I2C
  #if RUN_I2C
    Serial.begin(115200);
    Wire.begin(0x08);
    Wire.onReceive(receiveEvent);
  #endif

  //Debugging

}

//Code that handles I2C data
void receiveEvent(int howMany) {
  uint8_t packet[5];
  uint8_t idx = 0;
  Serial.print("[");
  while (Wire.available() > 0) {
    uint8_t data = Wire.read();
    packet[idx] = data;
    Serial.print(packet[idx]);
    Serial.print(",");
    idx += 1;
  }
  Serial.print("]");
    //If hand swipe, clear matrix
    if (packet[0] == 255 && packet[1] == 255){
      FastLED.clear(true);
      InitializeIndicatorLight(currentColor);
    }
    //Else update the current color for indicator light and update other light
    else{
      if(packet[2] != 0 && packet[3] != 0 && packet[4] != 0){
        currentColor.setRGB(packet[2], packet[3], packet[4]);
      }
      SetLEDX(packet);
  }
  Serial.println("");
}


void loop(){
  
}
