#include <Arduino.h>
#include <SPI.h>

#define SCLK_PIN 2   // Serial Clock
#define SIN_PIN 3    // Serial Data In
#define BLANK_PIN 4  // Blank
#define XLAT_PIN 5   // Latch
#define GSCLK_PIN 6  // Grayscale Clock
#define SOUT_PIN 8   // Serial Data Out
#define VPRG_PIN 9   // VPRG
#define DCPRG_PIN 10 // DCPRG

#define XLAT_DELAY 500
#define NUM_TLCS 1

#define pulse(pin, del)  do{\
                          digitalWrite(pin, HIGH); \
                          delayMicroseconds(del); \
                          digitalWrite(pin, LOW);\
                          }while(0)

uint8_t dcData[96] = {
  1, 1, 1, 1, 1, 1,  // Channel 15
  1, 1, 1, 1, 1, 1,  // Channel 14
  1, 1, 1, 1, 1, 1,  // Channel 13
  1, 1, 1, 1, 1, 1,  // Channel 12
  1, 1, 1, 1, 1, 1,  // Channel 11
  1, 1, 1, 1, 1, 1,  // Channel 10
  1, 1, 1, 1, 1, 1,  // Channel 9
  1, 1, 1, 1, 1, 1,  // Channel 8
  1, 1, 1, 1, 1, 1,  // Channel 7
  1, 1, 1, 1, 1, 1,  // Channel 6
  1, 1, 1, 1, 1, 1,  // Channel 5
  1, 1, 1, 1, 1, 1,  // Channel 4
  1, 1, 1, 1, 1, 1,  // Channel 3
  1, 1, 1, 1, 1, 1,  // Channel 2
  1, 1, 1, 1, 1, 1,  // Channel 1
  1, 1, 1, 1, 1, 1,  // Channel 0
};

uint8_t gsData[192 * NUM_TLCS] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Channel 15
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Channel 14
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Channel 13
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,  // Channel 12
  0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0,  // Channel 11
  0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,  // Channel 10
  0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,  // Channel 9
  0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,  // Channel 8
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Channel 7
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Channel 6
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Channel 5
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Channel 4
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Channel 3
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Channel 2
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // Channel 1
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // Channel 0
};
// SPISettings SPIsettings(11580, MSBFIRST, SPI_MODE0);

void setup() {
  // SPI.setRX(SOUT_PIN);
  // SPI.setSCK(SCLK_PIN);
  // SPI.setTX(SIN_PIN);
  pinMode(GSCLK_PIN, OUTPUT);
  pinMode(SCLK_PIN, OUTPUT);
  pinMode(SIN_PIN, OUTPUT);
  pinMode(XLAT_PIN, OUTPUT);
  pinMode(BLANK_PIN, OUTPUT);
  pinMode(SOUT_PIN, INPUT);
  pinMode(VPRG_PIN, OUTPUT);
  pinMode(DCPRG_PIN, OUTPUT);

  digitalWrite(GSCLK_PIN, LOW);
  digitalWrite(SCLK_PIN, LOW);
  digitalWrite(SIN_PIN, LOW);
  digitalWrite(XLAT_PIN, LOW);
  digitalWrite(BLANK_PIN, HIGH);
  digitalWrite(VPRG_PIN, HIGH);
  digitalWrite(DCPRG_PIN, LOW);
  // SPI.begin();

  //dotCorrection();
}

void loop() {
  // dotCorrection();
  digitalWrite(DCPRG_PIN, HIGH);
  delay(1000);
  digitalWrite(DCPRG_PIN, LOW);
}

void dotCorrection() {
  //Drive DCPRG and VPRG High
  digitalWrite(DCPRG_PIN, HIGH);
  uint8_t i;
  for (i = 0; i <= 95; i++) {
    writeToSIN(dcData[i]);
    // SPI.beginTransaction(SPIsettings);
    // SPI.transfer(&dcData[i], sizeof(dcData[i]));
    // SPI.endTransaction();
    delayMicroseconds(500);
    pulse(SCLK_PIN, 800);
  }
  delayMicroseconds(500);
  pulse(XLAT_PIN, XLAT_DELAY);
}

void grayscale() {
  //Drive VPRG Low
  uint8_t firstCycleFlag = 0;
  if (digitalRead(VPRG_PIN) == HIGH){
    digitalWrite(VPRG_PIN, LOW);
    firstCycleFlag = 1;
  }
  delayMicroseconds(500);
  uint16_t GSCLK_Counter = 0;
  uint8_t Data_Counter = 0;
  digitalWrite(BLANK_PIN, LOW);
  delayMicroseconds(500);
  for (GSCLK_Counter = 0; GSCLK_Counter < 4096; GSCLK_Counter++) {
    if (Data_Counter < NUM_TLCS * 192) {
      writeToSIN(gsData[Data_Counter]);
      // SPI.beginTransaction(SPIsettings);
      // SPI.transfer(&gsData[Data_Counter], sizeof(gsData[Data_Counter]));
      // SPI.endTransaction();      
      delayMicroseconds(500);
      pulse(SCLK_PIN, 500);
      Data_Counter += 1;
    }
    delayMicroseconds(500);
    pulse(GSCLK_PIN, 500);
    delayMicroseconds(500);
  }
  delayMicroseconds(500);
  digitalWrite(BLANK_PIN, HIGH);
  delayMicroseconds(500);
  pulse(XLAT_PIN, XLAT_DELAY);
  delayMicroseconds(500);
  if (firstCycleFlag == 1){
    pulse(SCLK_PIN, 500);
  }
  //digitalWrite(BLANK_PIN, HIGH);
  firstCycleFlag = 0;
}

void writeToSIN(uint8_t data) {
  if (data == (uint8_t) 1) {
    digitalWrite(SIN_PIN, HIGH);
  } 
  else {
    digitalWrite(SIN_PIN, LOW);
  }
  delayMicroseconds(750);
}