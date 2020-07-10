#include <TimeLib.h>
#include <Wire.h>
#include "SH1106Wire.h"
// Include the UI lib
#include "OLEDDisplayUi.h"
// Include custom images
#include "images.h"

SH1106Wire display(0x3c, 21, 22);
OLEDDisplayUi ui ( &display );


int screenW = 128;
int screenH = 64;
int clockCenterX = screenW / 2;
int clockCenterY = ((screenH - 32) / 2); // top yellow part is 16 px height

int potValues[] = {0, 0, 0};
int potPerc[] = {0, 0, 0};
String potLabels[] = {"Busuioc", "Menta", "Oregano"};
const byte potCount = 1;


const int AirValue = 3620;   //you need to replace this value with Value_1
const int WaterValue = 1680;  //you need to replace this value with Value_2

void digitalClockFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  String label = potLabels[state->currentFrame] + ':';
  String percentage = String(potPerc[state->currentFrame]) + '%';
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(clockCenterX + x , clockCenterY + y, label );
  display->drawProgressBar(clockCenterX + x - 50, clockCenterY + y + 18, 100, 6, potPerc[state->currentFrame]);
  display->drawString(clockCenterX + x , clockCenterY + y + 24, percentage );
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[potCount] = {};


// how many frames are there?
int frameCount = potCount;

const int selectPins[3] = {27, 14, 12};
void selectInput(byte pin) {
  if (pin > 7) return; // Exit if pin is out of scope
  for (int i=0; i<3; i++)
  {
    if (pin & (1<<i))
      digitalWrite(selectPins[i], HIGH);
    else
      digitalWrite(selectPins[i], LOW);
  }
}

void setup() {
  // Debug console
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  //  ui.setTargetFPS(60);
  // Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);
  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  ui.setIndicatorPosition(TOP);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  for (int i = 0; i < potCount; ++i) {
    frames[i] = digitalClockFrame;
  }
  ui.setFrames(frames, frameCount);


  // Initialising the UI will init the display too.
  ui.init();
  display.flipScreenVertically();

  delay(250);
  Serial.print("Setting pin ");
  Serial.print(A6);
  Serial.print(" to ");
  Serial.println(INPUT);
  pinMode(A6, INPUT);

  for (int i = 0; i < 3; i++) {
    Serial.print("Setting pin ");
    Serial.print(selectPins[i]);
    Serial.print(" to ");
    Serial.println(OUTPUT);

    pinMode(selectPins[i], OUTPUT);
    digitalWrite(selectPins[i], HIGH);
  }
}

void loop() {
  ui.update();
  for (byte i = 0; i < potCount; ++i) {
    selectInput(i);
    delay(250);
    potValues[i] = analogRead(A6);
    potPerc[i] = map(potValues[i], AirValue, WaterValue, 0, 100);
    Serial.print(potValues[i]);
    Serial.print("-");
    Serial.println(potPerc[i]);
  }

  delay(250);
}
