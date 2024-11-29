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

// buzzer pins
const int buzzerPin = 8;

// led pins
const int redLedPin = 7;
const int greenLedPin = 6;
const int yellowLedPin = 5;

// Adjustable by user
volatile int study = 30;
volatile int smallBreak = 30;
volatile int longBreak = 30;
volatile int interval = 4;

// subtracted from and reset
int studyTimer = study;
int smallBreakTimer = smallBreak;
int longBreakTimer = longBreak;


// used to determine if long break is next
int longBreakInterval = 0;

// number of times each timer had run
int studyCounter = 0;
int smallBreakCounter = 0;
int longBreakCounter = 0;

// study = 0, break = 1, long break = 2
int timer = 0;

// setup - runs once
void setup() {
  // print to serial monitor
  Serial.begin(9600);

  // led setup
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);

  // buzzer setup
  pinMode(buzzerPin, OUTPUT);

  // oled setup
  oled.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  Wire.begin();

  //Clear display 1
  selectDisplay(2, 0, 0, 3.5);
  oled.display();

  //Clear display 2
  selectDisplay(3, 0, 0, 3.5);
  oled.display();

  //Clear display 3
  selectDisplay(4, 0, 0, 3.5);
  oled.display();

  //Clear display 4
  selectDisplay(5, 0, 0, 3.5);
  oled.display();

  // displays timer counter and total times
  displaySessionStats();
}

bool iOnlyWantThisToPrintOnce = true;

// loops forever
void loop() {
  switch(timer) {
    // Study Timer case
    case 0:
      if (iOnlyWantThisToPrintOnce) {
        Serial.println("Start Session: " + String(studyCounter % 4) + " - " + String(millis()));
        iOnlyWantThisToPrintOnce = false;
      }
      //Serial.println("Start (Study Timer): " + String(millis()));
      //Serial.println("Should end (Study Timer): " + String(millis() + (study * 1000)));
      // ring buzzer :)
      ringBuzzer();

      while (studyTimer >= 0) {
        // display 1
        selectDisplay(2, 0, 0, 3.5);

        // print time
        oled.print(studyTimer / 60);
        oled.print(":");
        oled.println(studyTimer % 60);

        // actually show something on screen
        oled.display();

        // turn on red led
        digitalWrite(redLedPin, HIGH);

        // Delay to match 1 second
        delay(741);

        // update studyTimer
        studyTimer--;
      }

      // update longBreakInterval counter
      longBreakInterval++;

      // set timer value for next break type (either short break or long break)
      if (longBreakInterval == interval) {
        Serial.println("Switching to Long Break Timer");
        timer = 2;
      }
      else {
        Serial.println("Switching to Short Break Timer");
        timer = 1;
      }

      // reset timer for next time
      studyTimer = study;

      // update counter for calculations
      studyCounter++;

      // displays timer counter and total times
      displaySessionStats();

      // turn off red led
      digitalWrite(redLedPin, LOW);

      //Serial.println("End Case (Study Timer): " + String(millis()));

      break;

    // Short Break Timer case
    case 1:
      //Serial.println("Start (Short Break Timer): " + String(millis()));
      //Serial.println("Should end (Short Break Timer): " + String(millis() + (smallBreak * 1000)));

      // ring buzzer :)
      ringBuzzer();

      while (smallBreakTimer >= 0) {
        // display 2
        selectDisplay(3, 0, 0, 3.5);

        // print time
        oled.print(smallBreakTimer/60);
        oled.print(":");
        oled.println(smallBreakTimer%60);

        // actually show something on screen
        oled.display();

        // turn on green led
        digitalWrite(greenLedPin, HIGH);

        // Delay to match 1 second
        delay(739);

        // update smallBreakTimer
        smallBreakTimer--;
      }
    
      // set timer value for next studyTimer
      timer = 0;
      Serial.println("Switching to Study Timer");

      // reset timer for next time
      smallBreakTimer = smallBreak;

      // update counter for calculations
      smallBreakCounter++;

      // displays timer counter and total times
      displaySessionStats();

      // turn off green led
      digitalWrite(greenLedPin, LOW);

      //Serial.println("End Case (Short Break Timer): " + String(millis()));

      break;

    // Long Break Timer
    case 2:
      //Serial.println("Start (Long Break Timer): " + String(millis()));
      //Serial.println("Should end (Long Break Timer): " + String(millis() + (longBreak * 1000)));

      // ring buzzer :)
      ringBuzzer();

      while (longBreakTimer >= 0) {
        // display 3
        selectDisplay(4, 0, 0, 3.5);

        // print time
        oled.print(longBreakTimer/60);
        oled.print(":");
        oled.println(longBreakTimer%60);

        // actually show something on screen
        oled.display();

        // turn on yellow led
        digitalWrite(yellowLedPin, HIGH);
        
        // Delay to match 1 second
        delay(739);

        // update longBreakTimer
        longBreakTimer--;
      }
    
      // set timer value for next studyTimer
      timer = 0;
      Serial.println("Switching to Study Timer");

      // reset timer
      longBreakTimer = longBreak;
      
      // reset interval counter
      longBreakInterval = 0;

      // update counter for calculations
      longBreakCounter++;

      // displays timer counter and total times
      displaySessionStats();

      // Delay to match 1 second
      delay(10);

      // turn off yellow led
      digitalWrite(yellowLedPin, LOW);

      Serial.println("Session Done: " + String(millis()));
      iOnlyWantThisToPrintOnce = true;

      break;

    default:
      // Menu Code goes here
      // Choose timer to adjust
      // Start Timer
      break;
  }
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

void displaySessionStats() {
  selectDisplay(5, 0, 0, 1.5);
  
  String studyTotalTime = calculateTotalTime(study, studyCounter);
  String smallTotalTime = calculateTotalTime(smallBreak, smallBreakCounter);
  String longTotalTime = calculateTotalTime(longBreak, longBreakCounter);

  oled.println("Study: " + String(studyCounter) + " - " + studyTotalTime);
  oled.println("Short: " + String(smallBreakCounter) + " - " + smallTotalTime);
  oled.println("Long: " + String(longBreakCounter) + " - " + longTotalTime);
  oled.println("Interval: " + String(longBreakInterval));

  oled.display();
}

// time in seconds
String calculateTotalTime(int time, int timeCounter) {
  int seconds = time * timeCounter;
  int minutes = seconds / 60;
  int hours = minutes / 60;
  return String(int(hours)) + ":" + String(int(minutes%60)) + ":" + String(int(seconds%60));
}

void ringBuzzer() {
   // Send 1KHz sound signal
  tone(buzzerPin, 1000);

  // ring for 
  delay(125);   

  // Stop sound     
  noTone(buzzerPin);
}