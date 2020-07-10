#include <dummy.h>

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <TimeLib.h>
#include <Wire.h> 
#include "SH1106Wire.h"
// Include the UI lib
#include "OLEDDisplayUi.h"
// Include custom images
#include "images.h"


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

SH1106Wire display(0x3c, 21, 22);

OLEDDisplayUi ui ( &display );

int screenW = 128;
int screenH = 64;
int clockCenterX = screenW/2;
int clockCenterY = ((screenH-32)/2);   // top yellow part is 16 px height

int potValues[] = {0,0};
int potPerc[] = {0,0};
int potInputs[] = {A6,A7};
int potWrites[] = {V1, V2};
String potLabels[] = {"Busuioc", "Menta"};
const int potCount = 2;

void digitalClockFrame(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  String label = potLabels[state->currentFrame] + ':';
  String percentage = String(potPerc[state->currentFrame]) + '%';
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_16);
  display->drawString(clockCenterX + x , clockCenterY + y, label );
  display->drawProgressBar(clockCenterX + x - 50, clockCenterY + y + 18, 100, 6, potPerc[state->currentFrame]);
  display->drawString(clockCenterX + x , clockCenterY + y+24, percentage );
}

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[potCount] = {};


// how many frames are there?
int frameCount = potCount;

BlynkTimer timer;


// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  int remainingTimeBudget = ui.update();
 
  if (remainingTimeBudget > 0) {
    // You can send any value at any time.
    // Please don't send more that 10 values per second.
    for(int i = 0; i< potCount; ++i) {
        potValues[i] = analogRead(potInputs[i]);
        potPerc[i] = (4095 - potValues[i]) * 100 / 4095;
        Blynk.virtualWrite(potWrites[i], potPerc[i]);
    }

    delay(remainingTimeBudget);
  }
}

void setup()
{
  

  // Debug console
  Serial.begin(9600);
  ui.setTargetFPS(60);
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
  for(int i = 0; i<potCount;++i){
    frames[i] = digitalClockFrame;
  }
  ui.setFrames(frames, frameCount);
  
  
  // Initialising the UI will init the display too.
  ui.init();
  display.flipScreenVertically();
  
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);

  for(int i = 0; i<potCount; ++i){
    pinMode(potInputs[i], INPUT);
  }
  
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);

  
}


void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}
