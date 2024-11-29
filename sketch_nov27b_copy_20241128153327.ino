#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define OLED_Address 0x3C
Adafruit_SSD1306 oled(1);
#include "Wire.h"
#define TCAADDR 0x70
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

// Variables

// Adjustable by user
volatile int study = 30;
volatile int smallBreak = 30;
volatile int longBreak = 30;
volatile int interval = 4;

// subtracted from and reset
int studyTimer = study;
int smallBreakTimer = smallBreak;
int longBreakTimer = longBreak;
int longBreakInterval = interval;

// used to determine if long break is next
int longBreakCounter = 0;

// study = 0, break = 1, long break = 2
int timer = 0;

// setup - runs once
void setup() {
  // print to serial monitor
  Serial.begin(9600);

  // oled setup
  oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  Wire.begin();

  //Clear display 1
  tcaselect(2);
  oled.begin();
  oled.clearDisplay();
  oled.display();
  //Clear display 1
  tcaselect(3);
  oled.begin();
  oled.clearDisplay();
  oled.display();
}

// loops forever
void loop() {
  
  switch(timer) {
    // Study Timer case
    case 0:
      while (studyTimer >= 0) {
        // display 1
        selectDisplay(2, 0, 0, 3.5);

        // print time
        oled.print(studyTimer / 60);
        oled.print(":");
        oled.println(studyTimer % 60);

        // actually show something on screen
        oled.display();

        // update studyTimer
        studyTimer--;
      }

      // update longBreakIntervalCounter
      longBreakCounter++;

      // set timer value for next break type (either short break or long break)
      if (longBreakCounter == longBreakInterval) {
        Serial.println("Switching to Long Break Timer");
        timer = 2;
      }
      else {
        Serial.println("Switching to Short Break Timer");
        timer = 1;
      }

      // reset timer for next time
      studyTimer = study;

      break;

    // Short Break Timer case
    case 1:
      while (smallBreakTimer >= 0) {
        // display 2
        selectDisplay(3, 0, 0, 3.5);

        // print time
        oled.print(smallBreakTimer/60);
        oled.print(":");
        oled.println(smallBreakTimer%60);

        // actually show something on screen
        oled.display();

        // update smallBreakTimer
        smallBreakTimer--;
      }
    
      // set timer value for next studyTimer
      timer = 0;
      Serial.println("Switching to Study Timer");

      // reset timer for next time
      smallBreakTimer = smallBreak;

      break;

    // Long Break Timer
    case 2:
      while (longBreakTimer >= 0) {
        // display 3
        selectDisplay(4, 0, 0, 3.5);

        // print time
        oled.print(longBreakTimer/60);
        oled.print(":");
        oled.println(longBreakTimer%60);

        // actually show something on screen
        oled.display();

        // update longBreakTimer
        longBreakTimer--;
      }
    
      // set timer value for next studyTimer
      timer = 0;
      Serial.println("Switching to Study Timer");

      // reset timer for next time
      longBreakTimer = longBreak;

      break;

    default:
      // Menu Code goes here
      // Choose timer to adjust
      // Start Timer
      // Display timer times
      break;
  }
      
  // Delay to match 1 second
  delay(10);
}

// Functions

// write displays
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission(); 
}

// select display 
void selectDisplay(int display, int xCursor, int yCursor, float textSize) {
  tcaselect(display);
  oled.begin();
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(xCursor,yCursor);
  oled.setTextSize(textSize);
}