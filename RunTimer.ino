int pinBuzzer = 11;
int pinEcho = 14;
int pinTrig = 15;

// declare pins
int pinDigit[4] = {7, 4, 3, A7};
int pinSegment[7] = {6, 2, A3, A1, A0, 5, A4};

// create number templates using a 2D bit array
bool bNumber[10][7] = {
                        {1,1,1,1,1,1,0},
                        {0,1,1,0,0,0,0},
                        {1,1,0,1,1,0,1},
                        {1,1,1,1,0,0,1},
                        {0,1,1,0,0,1,1},
                        {1,0,1,1,0,1,1},
                        {1,0,1,1,1,1,1},
                        {1,1,1,0,0,0,0},
                        {1,1,1,1,1,1,1},
                        {1,1,1,1,0,1,1}
}; //

int timeDigits[4];

bool bWithinRange;

void setup() {
  Serial.begin(9600);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinTrig, OUTPUT);
  pinMode(pinEcho, INPUT);
                                
  for (int i = 0; i < 4; i++) {
    pinMode(pinDigit[i], OUTPUT);
  }

  for (int i = 0; i < 7; i++) {
    pinMode(pinSegment[i], OUTPUT);
  }
}

void loop() {
  // Main multiplexing loop.
  for (int i = 0; i < 4; i++) {
    setDigit(i);                // Select which digit we want to show.
    setNumber(timeDigits[i]);   // Display the desired digit for a single cycle.
    clearNumber();              // Hide digit before moving to the next to avoid afterglow.
  }

  // If the range is over 1m, run timer. Otherwise stop timer.
  if (!bWithinRange) {
    updateTimer();
  }

  // Run this last, so it doesn't interfere with multiplexing.
  checkRange();
}

// Send out a pulse, wait for it to come back. If it's less than 5.8ms (1m) runner is within range.
void checkRange() {
  unsigned long duration;
  bool bNewWithinRange;
  if (millis() % 100 == 0) {    
    // Pulse the sound for 10us.
    digitalWrite(pinTrig, LOW);
    delayMicroseconds(2);
    digitalWrite(pinTrig, HIGH);
    delayMicroseconds(10);
    digitalWrite(pinTrig, LOW);
    duration = pulseIn(pinEcho, HIGH, 5800);
    Serial.println(duration);
  }

  // If the timer state changes, ring the buzzer.
  if (bWithinRange != (duration != 0)) {
    tone(pinBuzzer, 5000, 300);
    bWithinRange = duration;
  }
}

/* Each digit of the LED display is a separate device. Thus, each digit must be populated separately.
 * I chained the update to a second. Make sure you include delay after update, because Arduino can run
 * multiple cycles in a single millisecond.
 */
void updateTimer() {
  if (millis() % 1000 == 0) {
    timeDigits[3]++;
    if (timeDigits[3] % 10 == 0) {
      timeDigits[3] = 0;
      timeDigits[2]++;
      if (timeDigits[2] % 6 == 0) {
        timeDigits[2] = 0;
        timeDigits[1]++;
        if (timeDigits[1] % 10 == 0) {
          timeDigits[1] = 0;
          timeDigits[0]++;
        }
      }
    }
  }
}

// Toggles on/off state for each digit. For multiplexing only one digit can glow at a time.
void setDigit(int digit) {
  for (int i = 0; i < 4; i++) {
    if (i == digit) {
      digitalWrite(pinDigit[i], LOW);
    } else {
      digitalWrite(pinDigit[i], HIGH);
    }
  }
}

// This loop pulls data from a pre-configured 2D array of digit configurations.
void setNumber(int number) {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinSegment[i], bNumber[number][i]);
  }
}

// Hides all segments.
void clearNumber() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(pinSegment[i], LOW);
  }
}
