#define pinBuz  15  // active buzzer
#define pinMicD 10  // digital microphone pin. Goes high when detects loud noise
#define pinCLK  9   // CLK pin on rotary encoder
#define pinDT   8   // DT pin on rotary encoder
#define pinSW   7   // rotary encoder momentary button

#include <Wire.h>             // used to run the OLED through I2C
#include <Adafruit_GFX.h>     // used for text
#include <Adafruit_SSD1306.h> // used to control the OLED

// Construct display, 128x64, using i2c bus, using Arduino reset pin
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Timers to be used in various sync tasks.
unsigned long timeLastBuz;
unsigned long timeLastDispUpdate;
unsigned long timeLastShot;
unsigned long timeStarted;

// Counters
int timerInterval = 2000;
int numShots;
int deltaShot;
int totalTime;
int averageTime;
int selectedMenuItem;

// Menu states
bool bChangingInterval;
bool bCountingShots;

void setup() {
  Serial.begin(115200); // Not necessary, used for debugging and diagnostics
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(pinBuz, OUTPUT);
  pinMode(pinMicD, INPUT);
  pinMode(pinCLK, INPUT);
  pinMode(pinDT, INPUT);
  pinMode(pinSW, INPUT_PULLUP);

  // Show adafruit logo on bootup
  display.display();
  delay(1000);
}

void loop() {
  handleRot();
  handleSW();
  countShots();
  drawMenu();
  
}

// Handles the rotary encoder
void handleRot() {
  int CLK = digitalRead(pinCLK);
  int DT = digitalRead(pinDT);

  // CLK is normally high. If it goes low, the encoder has moved in some direction.
  if (!CLK) {
    // Determine direction
    if (CLK == DT) { 
      // Are we changing the menu or setting the time interval?
      if (bChangingInterval) {
        timerInterval = constrain(timerInterval + 100, 300, 5000);
      } else {
        selectedMenuItem ++;
      }
    } else {
      if (bChangingInterval) {
        timerInterval = constrain(timerInterval - 100, 300, 5000);
      } else {
        selectedMenuItem --;
      }
    }
    // There is probably a cheaper way to do this than invoke constrain
    selectedMenuItem = constrain(selectedMenuItem, 0, 2);
    // Short delay is necessary if you have a cheap encoder. Probably could be fixed with a small resistor.
    delay(200);
  }
}

// Handle the push button
void handleSW() {
  int SW = digitalRead(pinSW);

  // Button is normally pulled up. It goes low when pushed.
  // This is designed like a mini-statemachine. If the state is x then do y.
  if (!SW) {
    switch(selectedMenuItem) {
      case 0: // adjusting interval
        bChangingInterval = !bChangingInterval;
        break;
      case 1: // starting interval. This is saved from repeated hits by delay. Normally should be in its own state.
        delay(2000);
        ringBuzzer(400);
        delay(timerInterval);
        ringBuzzer(800);
        break;
      case 2: // starting shot timer
        if (bCountingShots) {
          bCountingShots = false;
        } else {
          delay(2000);
          ringBuzzer(400);
          numShots = 0;
          timeStarted = millis();
          timeLastShot = millis();
          bCountingShots = true;
        }
        break;
    }
    // Again, delay is only necessary because of the quality of my button.
    delay(300);
  }
}

// Reads microphone data. If mic goes high, then handle the shot.
void countShots() {
  if (bCountingShots) {
    int micData = digitalRead(pinMicD); // Sensitivity of the mic is adjusted using the pot on the breakout.

    // If mic detected a shot, pin went high
    if (micData) {
      numShots ++;
      deltaShot = millis() - timeLastShot;
      timeLastShot = millis();
      averageTime = (millis() - timeStarted) / numShots;
      delay(50); // This will not work with automatic fire
    }
  }
}

// Prints menu
void drawMenu() {
  // Update display 10x/sec
  if (millis() - timeLastDispUpdate > 100)  {
    
    display.clearDisplay();
    display.setCursor(0, 0);

    // This is a very round-about way of changing the color of text and background to make it look selected
    // Comparing the selected menu item to the index. If they are the same then text goes dark, background goes light.
    display.setTextColor(selectedMenuItem != 0, selectedMenuItem == 0);
    display.setTextSize(2);  
    display.println(String(timerInterval) + "ms");
    
    
    // Draw menu
    display.setTextSize(1);
    display.setTextColor(selectedMenuItem != 1, selectedMenuItem == 1);
    display.println("START INTERVAL");
    display.setTextColor(selectedMenuItem != 2, selectedMenuItem == 2);
    display.println("START SHOT TIMER \n");

  if (bCountingShots) {
    display.setTextColor(1, 0);
    display.println("  last shot: " + String(deltaShot));
    display.println("  average time: " + String(averageTime));
    display.println("  total time: " + String(timeLastShot - timeStarted));
  }
    
    display.display();
    timeLastDispUpdate = millis();
  }
}

// Handle active buzzer. Frequency can be changed by changing delay between high and low cycle.
void ringBuzzer(int duration) {
  timeLastBuz = millis();
  while (millis() - timeLastBuz < duration) {
    digitalWrite(pinBuz, HIGH);
    delay(1);
    digitalWrite(pinBuz, LOW);
    delay(1);
  }
}
