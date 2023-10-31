#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0
#define SCLK_Pin 2
#define SIN_Pin 3
#define CS_Pin 5
#define SOUT_Pin 4
#define GSCLK_Pin 6
#define BLANK_Pin 7
#define XLAT_Pin 8

#include "MBED_RPi_Pico_TimerInterrupt.h"
#include <SPI.h>

#define NUM_TLCS 1
#define pulse_pin(pin)        digitalWrite(pin, HIGH); digitalWrite(pin, LOW)
#define TIMER_INTERVAL_US    15


SPISettings SPIsettings(11580, MSBFIRST, SPI_MODE0);
MbedSPI mySPI(SOUT_Pin,SIN_Pin,SCLK_Pin);
uint8_t tlc_GSData[NUM_TLCS * 24];
MBED_RPI_PICO_Timer ITimer1(1);
uint16_t GSCount = 1;

void TimerHandler(uint alarm_num)
{
  TIMER_ISR_START(alarm_num);
  if (GSCount < 4096) {
    pulse_pin(GSCLK_Pin);
    GSCount++;
  }
  else {
  }
  TIMER_ISR_END(alarm_num);
}

void GSCLK_Start() {
  if (ITimer1.attachInterruptInterval(TIMER_INTERVAL_US, TimerHandler))
  {
    Serial.print(F("Starting ITimer1 OK, millis() = ")); Serial.println(millis());
  }
  else 
  {
    Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
  }
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(5000);
  GSCLK_Start();
  setAll(2000);
  mySPI.beginTransaction(SPIsettings);
  mySPI.transfer(tlc_GSData, sizeof(tlc_GSData));
  mySPI.endTransaction();
  digitalWrite(BLANK_Pin,HIGH);
  pulse_pin(XLAT_Pin);
  pulse_pin(SCLK_Pin);
  // digitalWrite(BLANK_Pin,LOW);

}

void loop() {
  // put your main code here, to run repeatedly:
  

}
