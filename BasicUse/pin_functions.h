#ifndef TLC_pin_functions_h
#define TLC_pin_functions_h

  #define pulse_pin(pin)        digitalWrite(pin, HIGH); digitalWrite(pin, LOW)
  #define set_pin(pin)          digitalWrite(pin, HIGH)
  #define clear_pin(pin)        digitalWrite(pin, LOW)
  #define output_pin(pin)        pinMode(pin, OUTPUT)
  #define pullup_pin(pin)  pinMode(pin, INPUT_PULLUP)

#endif
