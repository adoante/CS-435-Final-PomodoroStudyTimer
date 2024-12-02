#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Reset pin # (or -1 if sharing Arduino reset pin), required to be set
#define OLED_RESET     -1
#define OLED_Address 0x3C
Adafruit_SSD1306 oled(1);
#include "Wire.h"
#define TCAADDR 0x70
// OLED display width, in pixels
#define SCREEN_WIDTH 128
// OLED display height, in pixels
#define SCREEN_HEIGHT 64

// Variables

// buzzer pins
const int buzzerPin = 8;

// button pins
const int menuButtonPin = 2;
const int startStopButtonPin = 3;
const int incButtonPin = 10;
const int decButtonPin = 11;

// button anti-debounce variables
// millies() returned unsinged long
unsigned long menuButtonPrevPress = 0;
unsigned long startStopButtonPrevPress = 0;
unsigned long incButtonPrevPress = 0;
unsigned long decButtonPrevPress = 0;

unsigned long menuButtonCurrPress = 0;
unsigned long startStopButtonCurrPress = 0;
unsigned long incButtonCurrPress = 0;
unsigned long decButtonCurrPress = 0;

// button delay between presses, milliseconds
unsigned long debounceDelay = 50;

// led pins
const int redLedPin = 7;
const int greenLedPin = 6;
const int yellowLedPin = 5;

// Adjustable by user
volatile int study = 60;
volatile int shortBreak = 60;
volatile int longBreak = 60;
volatile int interval = 4;

// subtracted from and reset
int studyTimer = study;
int shortBreakTimer = shortBreak;
int longBreakTimer = longBreak;

// used to determine if long break is next
int longBreakInterval = 0;

// number of times each timer had run
int studyCounter = 0;
int shortBreakCounter = 0;
int longBreakCounter = 0;

// menu (default) = -1, study = 0, break = 1, long break = 2
volatile int timer = -1;

// welcome (default) = -1, study = 0, break = 1, long break = 3
volatile int menu = -1;

// setup - runs once
void setup() {
  // print to serial monitor
  Serial.begin(9600);

  // interrupt button setup
  pinMode(menuButtonPin, INPUT);
  pinMode(startStopButtonPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(menuButtonPin), changeMenuItem, FALLING);
  attachInterrupt(digitalPinToInterrupt(startStopButtonPin), startStopTimers, FALLING);

  // inc. dec. button setup
  pinMode(incButtonPin, INPUT);
  pinMode(decButtonPin, INPUT);

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

  //Clear display 5
  selectDisplay(6, 0, 0, 3.5);
  oled.display();

  // Start Screens

  selectDisplay(2, 14, 0, 2);
  oled.println("Pomodoro");
  oled.println("StudyTimer");
  oled.display();

  selectDisplay(3, 0, 0, 2);

  displaySessionStats();

  displayMenu();
}

bool iOnlyWantThisToPrintOnce = true;

// loops forever
void loop() {
  switch(timer) {
    // Study Timer case
    case 0:
      
      //Clear display 2
      selectDisplay(3, 0, 0, 3.5);
      oled.println("Study");
      oled.display();

      //Clear display 3
      selectDisplay(4, 0, 0, 3.5);
      oled.println("Time!");
      oled.display();

      // ring buzzer :)
      ringBuzzer();

      while (studyTimer >= 0) {
        // stop timer, return to case
        if (timer == -1) {
          break;
        }
        // display 1
        selectDisplay(2, 0, 0, 3.5);

        // print time
        displayTimer(studyTimer);

        // actually show something on screen
        oled.display();

        // turn on red led
        digitalWrite(redLedPin, HIGH);

        // Delay to match 1 second
        delay(741);

        // update studyTimer
        studyTimer--;
      }

      // stop timer, return to menu
      if (timer == -1) {
          break;
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
      //Clear display 1
      selectDisplay(2, 0, 0, 3.5);
      oled.println("Break");
      oled.display();

      //Clear display 3
      selectDisplay(4, 0, 0, 3.5);
      oled.println("Time!");
      oled.display();

      // set time
      shortBreakTimer = shortBreak;

      // ring buzzer :)
      ringBuzzer();

      while (shortBreakTimer >= 0) {
        // stop timer, return to case
        if (timer == -1) {
          break;
        }

        // display 2
        selectDisplay(3, 0, 0, 3.5);

        // print time
        displayTimer(shortBreakTimer);

        // actually show something on screen
        oled.display();

        // turn on green led
        digitalWrite(greenLedPin, HIGH);

        // Delay to match 1 second
        delay(739);

        // update shortBreakTimer
        shortBreakTimer--;
      }

      // stop timer, return to case
      if (timer == -1) {
        break;
      }
    
      // set timer value for next studyTimer
      timer = 0;
      Serial.println("Switching to Study Timer");

      // reset timer for next time
      shortBreakTimer = shortBreak;

      // update counter for calculations
      shortBreakCounter++;

      // displays timer counter and total times
      displaySessionStats();

      // turn off green led
      digitalWrite(greenLedPin, LOW);

      //Serial.println("End Case (Short Break Timer): " + String(millis()));

      break;

    // Long Break Timer
    case 2:
      //Clear display 1
      selectDisplay(2, 0, 0, 1.5);
      oled.println("Long");
      oled.setTextSize(3.5);
      oled.println("Break");
      oled.display();

      //Clear display 2
      selectDisplay(3, 0, 0, 3.5);
      oled.println("Time!");
      oled.display();

      // set time
      longBreakTimer = longBreak;

      // ring buzzer :)
      ringBuzzer();

      while (longBreakTimer >= 0) {
        // stop timer, return to case
        if (timer == -1) {
          break;
        }

        // display 3
        selectDisplay(4, 0, 0, 3.5);

        // print time
        displayTimer(longBreakTimer);

        // actually show something on screen
        oled.display();

        // turn on yellow led
        digitalWrite(yellowLedPin, HIGH);
        
        // Delay to match 1 second
        delay(739);

        // update longBreakTimer
        longBreakTimer--;
      }

      // stop timer, return to menu
      if (timer == -1) {
        break;
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
      displayMenu();

      switch(menu) {
        case 0:
          // check if button input is valid
          incButtonCurrPress = millis();

          // current button press must be 'debounceDelay' ms after previous button press
          if (incButtonCurrPress - incButtonPrevPress > debounceDelay) {
            if (digitalRead(incButtonPin) == HIGH && study < 3600) {
              study = study + 60;
            }

            incButtonPrevPress = incButtonCurrPress;
          }

          // check if button input is valid
          decButtonCurrPress = millis();

          // current button press must be 'debounceDelay' ms after previous button press
          if (decButtonCurrPress - decButtonPrevPress > debounceDelay) {
            if (digitalRead(decButtonPin) == HIGH && study > 0) {
              study = study - 60;
            }

            decButtonPrevPress = decButtonCurrPress;
          }

          // update timer 
          studyTimer = study;

          break;
        case 1:
          // check if button input is valid
          incButtonCurrPress = millis();

          // current button press must be 'debounceDelay' ms after previous button press
          if (incButtonCurrPress - incButtonPrevPress > debounceDelay) {
            if (digitalRead(incButtonPin) == HIGH && shortBreak < 3600) {
              shortBreak = shortBreak + 60;
            }

            incButtonPrevPress = incButtonCurrPress;
          }

          // check if button input is valid
          decButtonCurrPress = millis();

          // current button press must be 'debounceDelay' ms after previous button press
          if (decButtonCurrPress - decButtonPrevPress > debounceDelay) {
            if (digitalRead(decButtonPin) == HIGH && shortBreak > 0) {
              shortBreak = shortBreak - 60;
            }

            decButtonPrevPress = decButtonCurrPress;
          }

          // update timer 
          shortBreakTimer = shortBreak;

          break;
        case 2:
          // check if button input is valid
          incButtonCurrPress = millis();

          // current button press must be 'debounceDelay' ms after previous button press
          if (incButtonCurrPress - incButtonPrevPress > debounceDelay) {
            if (digitalRead(incButtonPin) == HIGH && longBreak < 3600) {
              longBreak = longBreak + 60;
            }

            incButtonPrevPress = incButtonCurrPress;
          }

          // check if button input is valid
          decButtonCurrPress = millis();

          // current button press must be 'debounceDelay' ms after previous button press
          if (decButtonCurrPress - decButtonPrevPress > debounceDelay) {
            if (digitalRead(decButtonPin) == HIGH && longBreak > 0) {
              longBreak = longBreak - 60;
            }

            decButtonPrevPress = decButtonCurrPress;
          }

          // update timer 
          longBreakTimer = longBreak;

          break;
        case 3:
          // check if button input is valid
          incButtonCurrPress = millis();

          // current button press must be 'debounceDelay' ms after previous button press
          if (incButtonCurrPress - incButtonPrevPress > debounceDelay) {
            // update interval
            if (digitalRead(incButtonPin) == HIGH) {
              interval++;
            }

            incButtonPrevPress = incButtonCurrPress;
          }

          // check if button input is valid
          decButtonCurrPress = millis();

          // current button press must be 'debounceDelay' ms after previous button press
          if (decButtonCurrPress - decButtonPrevPress > debounceDelay) {
            // update interval
            if (digitalRead(decButtonPin) == HIGH && interval > 0) {
              interval--;
            }
            
            decButtonPrevPress = decButtonCurrPress;
          }
          break;
        default:
          
          break;
      }
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
  String shortTotalTime = calculateTotalTime(shortBreak, shortBreakCounter);
  String longTotalTime = calculateTotalTime(longBreak, longBreakCounter);
  int totalTimeX = study * studyCounter;
  int totalTimeY = shortBreak * shortBreakCounter;
  int totalTimeZ = longBreak * longBreakCounter;
  String totalTime = calculateTotalTime((totalTimeX + totalTimeY + totalTimeZ), 1);

  oled.println("Study   : " + String(studyCounter) + " - " + studyTotalTime);
  oled.println("Short   : " + String(shortBreakCounter) + " - " + shortTotalTime);
  oled.println("Long    : " + String(longBreakCounter) + " - " + longTotalTime);
  oled.println("Interval: " + String(longBreakInterval) + " - " + totalTime);

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

// Changes menu item
// Fix debounce problem
void changeMenuItem() {
  // check if button input is valid
  menuButtonCurrPress = millis();

  // current button press must be 'debounceDelay' ms after previous button press
  if (menuButtonCurrPress - menuButtonPrevPress > debounceDelay) {
    menu = (menu + 1) % 4;
    menuButtonPrevPress = menuButtonCurrPress;
  }
}

// Starts timer or stops timer (goes to menu)
void startStopTimers() {
  // check if button input is valid
  startStopButtonCurrPress = millis();

  // current button press must be 'debounceDelay' ms after previous button press
  if (startStopButtonCurrPress - startStopButtonPrevPress > debounceDelay) {

    if (timer == -1) {
      timer = 0;
    }
    else {
      timer = -1;
    }

    // reset leds
    digitalWrite(redLedPin, LOW);
    digitalWrite(yellowLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
  }
}

void displayTimer(int timerType) {
  int min = timerType / 60;
  int sec = timerType % 60;

  if (min >= 10 && min <= 99) {
    oled.print(String(min));
  }
  else {
    oled.print("0" + String(min));
  }
  
  oled.print(":");
  
  if (sec >= 10 && sec <= 99) {
    oled.print(String(sec));
  }
  else {
    oled.print("0" + String(sec));
  }
}

void displayMenu() {

  selectDisplay(6, 0, 0, 1.5);
  
  switch (menu) {
    // study option
    case 0:
      oled.print("* Study   : ");
      displayTimer(study);
      oled.println();

      oled.print("  Short   : ");
      displayTimer(shortBreak);
      oled.println();

      oled.print("  Long    : ");
      displayTimer(longBreak);
      oled.println();

      oled.print("  Interval: " + String(interval));

      oled.display();
      break;

    // short break option
    case 1:
      oled.print("  Study   : ");
      displayTimer(study);
      oled.println();

      oled.print("* Short   : ");
      displayTimer(shortBreak);
      oled.println();

      oled.print("  Long    : ");
      displayTimer(longBreak);
      oled.println();

      oled.print("  Interval: " + String(interval));

      oled.display();
      break;
    
    // long break option
    case 2:
      oled.print("  Study   : ");
      displayTimer(study);
      oled.println();

      oled.print("  Short   : ");
      displayTimer(shortBreak);
      oled.println();

      oled.print("* Long    : ");
      displayTimer(longBreak);
      oled.println();

      oled.print("  Interval: " + String(interval));

      oled.display();
      break;

    // interval option
    case 3:
      oled.print("  Study   : ");
      displayTimer(study);
      oled.println();

      oled.print("  Short   : ");
      displayTimer(shortBreak);
      oled.println();

      oled.print("  Long    : ");
      displayTimer(longBreak);
      oled.println();

      oled.print("* Interval: " + String(interval));

      oled.display();
      break;

    default:
      oled.print("  Study   : ");
      displayTimer(study);
      oled.println();

      oled.print("  Short   : ");
      displayTimer(shortBreak);
      oled.println();

      oled.print("  Long    : ");
      displayTimer(longBreak);
      oled.println();

      oled.print("  Interval: " + String(interval));

      oled.display();
      break;
  }
}