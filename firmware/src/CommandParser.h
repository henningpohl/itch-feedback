#pragma once

struct SignalParameters {
  uint32_t frequency;
  uint32_t amplitude;
  uint32_t pulseWidth;

  // Default: 50Hz, 2ms pulse width, poti to the max 
  SignalParameters() : frequency(50), amplitude(0), pulseWidth(2000) {} 
};

void onParameterChange(SignalParameters & params);
void onEnableChange(bool enable);
#if INCLUDE_TESTS
  void selectTestMode(uint32_t testMode);
  typedef void (* LoopFunction)();
  extern LoopFunction loopTestFunction;
#endif

uint32_t * currentField = nullptr;
SignalParameters signalParams;
char currentMethod = ' ';
#if INCLUDE_TESTS
  uint32_t testMode = 0;
#endif
uint32_t enableState = 0;


void finalizeCommand() {
#if INCLUDE_TESTS
  loopTestFunction = nullptr;
#endif

  if(currentMethod == 'F' || currentMethod == 'A' || currentMethod == 'W') {
    onParameterChange(signalParams);
    Serial.println("OK");
  } else if(currentMethod == '*') {
    onEnableChange(true);
    enableState = 1;
    Serial.println("OK");
  } else if(currentMethod == '#') {
    onEnableChange(false);
    enableState = 0;
    Serial.println("OK");
  } else if(currentMethod == 'N') {
    Serial.println(BOARD_NAME);
  } else if(currentMethod == 'S') {
    Serial.println(BOARD_SPECS);
  } else if(currentMethod == 'P') {
    Serial.print(signalParams.frequency);
    Serial.print(",");
    Serial.print(signalParams.amplitude);
    Serial.print(",");
    Serial.print(signalParams.pulseWidth);
    Serial.print(",");
    Serial.print(enableState);
  } 
#if INCLUDE_TESTS
  else if(currentMethod == 'T') {
    selectTestMode(testMode);
    Serial.println("OK");
  }
#endif  
  else if(currentMethod == '?') {
    Serial.println("Send 'F##' to change frequency (hertz)");
    Serial.println("Send 'W##' to set pulse width (nanoseconds)");
    Serial.println("Send 'A##' to set amplitude (raw poti value)");
    Serial.println("Send 'T##' to select a test mode");
    Serial.println("Send 'N' to query board name");
    Serial.println("Send 'S' to query board specs");
    Serial.println("Send 'P' to query current parameters/status");
    Serial.println("Send '*' to turn on and '#' to turn off the system");
    Serial.println("Terminate commands with newline character");
  } else {
    Serial.println("ERR");
  }

  currentField = nullptr;
}

void processByte(const uint8_t c) {
  if ('0' <= c && c <= '9' && currentField != nullptr) {
    *currentField = ((*currentField) * 10) + (c - '0');
    return;
  }

  switch (c) {
  case 'F':
    currentField = &signalParams.frequency;
    *currentField = 0;
    currentMethod = 'F';
    break;
  case 'A':
    currentField = &signalParams.amplitude;
    *currentField = 0;
    currentMethod = 'A';
    break;
  case 'W':
    currentField = &signalParams.pulseWidth;
    *currentField = 0;
    currentMethod = 'W';
    break;
#if INCLUDE_TESTS
  case 'T':
    currentMethod = 'T';
    currentField = &testMode;
    *currentField = 0;
    break;
#endif
  case '\n':
    finalizeCommand();
    currentMethod = ' ';
    currentField = nullptr;
    break;
  case '*':
  case '#':
  case 'N':
  case 'S':
  case 'P':
  case '?':
    currentMethod = c;
    currentField = nullptr;
    break;
  default:
    currentMethod = ' ';
    currentField = nullptr;
    break;
  }
}
