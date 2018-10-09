#include <Arduino.h>

#define BOARD_VARIANT 1
#if BOARD_VARIANT == 1
  #define BOARD_NAME "Left Board"
  #define BOARD_SPECS "5000 dynamic,0 fixed,255 steps"
#elif BOARD_VARIANT == 2
  #define BOARD_NAME "Right Board"
  #define BOARD_SPECS "5000 dynamic,0 fixed,255 steps"
#else
  #error Invalid board variant
#endif

#define INCLUDE_TESTS 1

#include "SignalController.h"
#include "CommandParser.h"

void onEnableChange(bool enable) {
  if(enable) {
    turnOnSignal();
    //digitalWrite(LED_BUILTIN, HIGH);
  } else {
    turnOffSignal();
    //digitalWrite(LED_BUILTIN, LOW);
  }
}

void setup() {
  // Uncomment lines below to wait for IDE connection on start
  //while (!Serial);
  //delay(500);
  Serial.begin(115200);
  Serial.println("Connected: send '?' for help");

  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
  setupSignal();
}



void loop() {
  while(Serial.available() > 0) {
    processByte(Serial.read());
  }

#if INCLUDE_TESTS
  if(loopTestFunction) {
    loopTestFunction();
  }
#endif
  delay(100);
}