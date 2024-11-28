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

// write displays
void tcaselect(uint8_t i) {
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission(); 
}

const byte ledPin = 7;
const byte interruptPin = 3;
volatile byte state = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, RISING);

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

volatile int cnt = 300;
volatile int studyTimer = 3000;
volatile int smallBreakTimer = 0;
volatile int longBreakInterval = 4;
volatile int longBreakTimer = 900;

void loop() {
  tcaselect(2); // Text to Oled display 1
  oled.begin();
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.setTextSize(3.5);
  if(studyTimer == -1){
    oled.print("0:00");
  }
  else {
    oled.print(studyTimer/60);
    oled.print(":");
    oled.println(studyTimer%60);
  }
  //(cnt/60)+":"+(cnt%60)
  if(studyTimer > 0){
    studyTimer--;
  }
  
  oled.display();

  //Break timer
  tcaselect(3);
  oled.begin();
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.setTextSize(3.5);
  if(smallBreakTimer == -1){
    oled.print("0:00");
  }
  else {
    oled.print(smallBreakTimer/60);
    oled.print(":");
    oled.println(smallBreakTimer%60);
  }
  if(smallBreakTimer > 0){
    smallBreakTimer--;
  }
  oled.display();

  //Long Break timer
  tcaselect(4);
  oled.begin();
  oled.clearDisplay();
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.setTextSize(3.5);
  if(smallBreakTimer == -1){
    oled.print("0:00");
  }
  oled.display();

  delay(10);

  if (longBreakInterval = 0) {
    
  }

  if(studyTimer == 0)
    {
      smallBreakTimer = 3000;
      studyTimer--;
    }
  if(smallBreakTimer == 0)
    {
      studyTimer = 3000;
      smallBreakTimer--;
    }
    
}

void blink() {
  state = !state;
  digitalWrite(ledPin, state);
  studyTimer++;
}