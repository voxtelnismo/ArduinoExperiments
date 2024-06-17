/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

float cellVoltage[4] = {3, 6, 9, 12}; // Voltage read and normalied
float cellMultiplier[4] = {1.0, 1.99, 2.81, 3.72}; // Factor by which we need to multiply the reading

void setup() {

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); // Pause for 1 second

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
}

void loop() {
display.clearDisplay();

getCellVoltage();
drawCellVoltage();
display.display();
}

// Read voltage from ADC, normalize it against 5V reference, then multiply it by appropriate voltage divider value, and run through a low pass filter.
void getCellVoltage() {
  cellVoltage[0] = cellVoltage[0] * 0.9 + (analogRead(A7) * 5 / 1023.0f * cellMultiplier[0]) * 0.1;
  cellVoltage[1] = cellVoltage[1] * 0.9 + (analogRead(A6) * 5 / 1023.0f * cellMultiplier[1]) * 0.1;
  cellVoltage[2] = cellVoltage[2] * 0.9 + (analogRead(A3) * 5 / 1023.0f * cellMultiplier[2]) * 0.1;
  cellVoltage[3] = cellVoltage[3] * 0.9 + (analogRead(A2) * 5 / 1023.0f * cellMultiplier[3]) * 0.1;
}

void drawCellVoltage() {
  display.setCursor(0, 0);
  display.println("Battery voltage:");

  for (int i = 3; i > 1; i--) {
    if (cellVoltage[i] > 1) {
      display.println(cellVoltage[i]);
      break;
    }
  }

  display.println(cellVoltage[0]);
  display.println(cellVoltage[1] - cellVoltage[0]);
  if (cellVoltage[2] > 1) {display.println(cellVoltage[2] - cellVoltage[1]);}
  if (cellVoltage[3] > 1) {display.println(cellVoltage[3] - cellVoltage[2]);}
}

