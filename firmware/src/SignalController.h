#pragma once

#include "Timer5.h"
#include "MCP41HV.h"
#include "CommandParser.h"

const int SIGNAL_A = 11;
const int SIGNAL_B = 10;
const int BOOST_ENABLE = A2;

const int POTI_CS = A4;
const int POTI_WLAT = 13;
const int POTI_SHDN = 12;
MCP41HV poti(POTI_CS);

bool isTurnedOn = false;

#if INCLUDE_TESTS
  typedef void (* LoopFunction)();
  LoopFunction loopTestFunction = nullptr;
#endif


const uint16_t TIMER_TICKS = 32768;
const double MICROSECONDS_TO_TICKS = TIMER_TICKS / 1000000.0;
uint16_t signalToggle = 655; // 50Hz
uint16_t pulseToggle = 65; // 2ms pulse
uint16_t curTimerState = 0;

void Timer5_IRQ() {
  if(!isTurnedOn) {
    return;
  }

  if(curTimerState == 0) {
    digitalWrite(SIGNAL_A, HIGH);
  } else if(curTimerState == pulseToggle) {
    digitalWrite(SIGNAL_A, LOW);
  } else if(curTimerState == pulseToggle + 20) {
    digitalWrite(SIGNAL_B, HIGH);
  } else if(curTimerState == (2 * pulseToggle + 20)) {
    digitalWrite(SIGNAL_B, LOW);
  }
  
  curTimerState = (curTimerState + 1) % (signalToggle + 1);
}

void onParameterChange(SignalParameters & params) {
  poti.writeWiperValue(params.amplitude);
  signalToggle = TIMER_TICKS / params.frequency;
  pulseToggle = params.pulseWidth * MICROSECONDS_TO_TICKS; 
  curTimerState = 0;
}

void setupSignal() {
  pinMode(POTI_WLAT, OUTPUT);
  pinMode(POTI_SHDN, OUTPUT);
  pinMode(SIGNAL_A, OUTPUT);
  pinMode(SIGNAL_B, OUTPUT);
  pinMode(BOOST_ENABLE, OUTPUT);

  poti.setup();
  SPI.begin();

  MyTimer5.begin(1); // run at full 32768 Hz
  MyTimer5.attachInterrupt(Timer5_IRQ);

  digitalWrite(BOOST_ENABLE, LOW);
  digitalWrite(POTI_WLAT, LOW); // Don't delay wiper value updates
  digitalWrite(POTI_SHDN, LOW); // Shut down initially
  digitalWrite(SIGNAL_A, LOW);
  digitalWrite(SIGNAL_B, LOW);
}

void turnOnSignal() {
#if INCLUDE_TESTS
  loopTestFunction = nullptr;
#endif

  isTurnedOn = true;
  digitalWrite(BOOST_ENABLE, HIGH);
  digitalWrite(POTI_SHDN, HIGH);
  MyTimer5.begin(1); // run at full 32768 Hz
  MyTimer5.start();
}

void turnOffSignal() {
#if INCLUDE_TESTS
  loopTestFunction = nullptr;
#endif

  isTurnedOn = false;
  digitalWrite(BOOST_ENABLE, LOW);
  digitalWrite(POTI_SHDN, LOW);
  MyTimer5.end();
}

#if INCLUDE_TESTS
void constantTestSignal(uint8_t strength) {
  isTurnedOn = false;
  MyTimer5.end();
  digitalWrite(BOOST_ENABLE, HIGH);
  digitalWrite(POTI_SHDN, HIGH);
  poti.writeWiperValue(strength);
  digitalWrite(SIGNAL_A, LOW);
  digitalWrite(SIGNAL_B, HIGH);

}

uint8_t rampTestState;
void rampLoopFunction() {
  poti.writeWiperValue(rampTestState);
  rampTestState--;
  digitalWrite(SIGNAL_A, LOW);
  digitalWrite(SIGNAL_B, HIGH);
  delayMicroseconds(2000);
  digitalWrite(SIGNAL_A, LOW);
  digitalWrite(SIGNAL_B, LOW);
}

void rampTestSignal() {
  isTurnedOn = false;
  MyTimer5.end();
  digitalWrite(BOOST_ENABLE, HIGH);
  digitalWrite(POTI_SHDN, HIGH);
  digitalWrite(SIGNAL_A, LOW);
  digitalWrite(SIGNAL_B, LOW);
  rampTestState = 255;
  poti.writeWiperValue(rampTestState);
  loopTestFunction = rampLoopFunction;
}

void testSignalOutput() {
  digitalWrite(POTI_SHDN, HIGH);
  poti.writeWiperValue(234); // should be 1.5mA -> 15V over the 10k resistor
  isTurnedOn = true;
  MyTimer5.begin(1); // run at full 32768 Hz
  MyTimer5.start();
}

void testSignalOutput2() {
  digitalWrite(POTI_SHDN, HIGH);
  poti.writeWiperValue(220);
  digitalWrite(SIGNAL_B, HIGH);
  delay(2000);
  digitalWrite(SIGNAL_B, LOW);
  delay(2000);
  digitalWrite(SIGNAL_A, HIGH);
  delay(2000);
  digitalWrite(SIGNAL_A, LOW);
  delay(2000);
}

void selectTestMode(uint32_t testMode) {
  switch(testMode) {
    case 0:
      constantTestSignal(255);
      break;
    case 1:
      constantTestSignal(200);
      break;
    case 2:
      constantTestSignal(150);
      break;
    case 3:
      rampTestSignal();
      break;
  }
}
#endif