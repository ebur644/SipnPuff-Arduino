#include <string.h>
#include <stdlib.h>
#include "HX710.h"


#include <BleKeyboard.h>

BleKeyboard bleKeyboard;



////////////////////////////////////////////////////////////////
// PARAMETERS
////////////////////////////////////////////////////////////////

const int DOUT = 20;
const int PD_SCK = 8;
int32_t value;
int32_t bar;
int SipThreshold = -50;
int PuffThreshold = 50;
char modeselect = 'G';





////////////////////////////////////////////////////////////////
// SETUP
////////////////////////////////////////////////////////////////

HX710 ps;

void setup() {
  pinMode(20, INPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);
  bleKeyboard.begin();
  
  ps.initialize(PD_SCK, DOUT);
}

////////////////////////////////////////////////////////////////
// LOOP
////////////////////////////////////////////////////////////////

void loop() {


  while (!ps.isReady())
    ;

  ps.readAndSelectNextData(HX710_DIFFERENTIAL_INPUT_40HZ);
  value = ps.getLastDifferentialInput();
  if (value == 100) {

  } else if (value <= -141581) {
    bar = map(value, -1415815, -8388608, 0, -100);
  } else if (value <= 2367489) {
    bar = map(value, -1415815, 2367489, 0, 100);
  } else if (value > 2367489) {
    bar = 100;
  }
  Mode();
}

void Mode(){
  switch (modeselect){
    case 'G':
      ArrowKeys();
    break;
    case 'C':
      Calabrate();
    break;
    case 'E':

    break;
    default:

    break;
  }
}

void Calabrate(){
  if(Serial.available() > 0){
    modeselect = Serial.readUntil('\n');
    SipThreshold = Serial.readUntil('\n');
    SipThreshold = Serial.readUntil('\n');
  }
  Serial.println(bar);
}

void Engineering(){
  Serial.println(value);
}

void ArrowKeys() {
  static int previousRange = -1;  // Initialize with a value that won't be a valid range

  if (bleKeyboard.isConnected()) {
    int currentRange = -1;  // Initialize with a value that won't be a valid range

    if (bar >= -100 && bar <= SipThreshold) {
      // Bar is within the range of -50 to -5
      currentRange = 0;
    } else if (bar >= SipThreshold && bar <= -5) {
      // Bar is within the range of -5 to 5
      currentRange = 1;
    } else if (bar >= -5 && bar <= 5) {
      // Bar is within the range of -5 to 5
      currentRange = 2;
    } else if (bar >= 5 && bar <= PuffThreshold) {
      // Bar is within the range of 5 to 50
      currentRange = 3;
    } else if (bar >= PuffThreshold && bar <= 100) {
      // Bar is within the range of 50 to 100
      currentRange = 4;
    }

    if (currentRange != previousRange) {
      bleKeyboard.releaseAll();
      previousRange = currentRange;
    }

    if (currentRange == 0) {
      bleKeyboard.press(KEY_DOWN_ARROW);
    } else if (currentRange == 1) {
      bleKeyboard.press(KEY_RIGHT_ARROW);
    } else if (currentRange == 2) {
      bleKeyboard.releaseAll();
    } else if (currentRange == 3) {
      bleKeyboard.press(KEY_LEFT_ARROW);
    } else if (currentRange == 4) {
      bleKeyboard.press(KEY_UP_ARROW);
    }
  }
}
