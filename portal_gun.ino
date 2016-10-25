// Code for some Rick and Morty portal gun fun
// Dan Kouba, 2016

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

// 32 sample log sine wave table, since brightness is logarithmic
// Yes I know the top and bottom points repeat
const int logSineTable[32] = {
  0,  1,  2,  3,  5,  8,   11,  16,
  23, 32, 45, 64, 90, 127, 180, 255,
  255, 180, 127, 90, 64, 45, 32, 23,
  16, 11, 8,  5,  3,  2,   1,   0
};

// Fade out/in functions - not ideal since we only have 8 steps...brightness 8 on the display is quite bright though!
void fadeout() {
  for (int i = 7; i >=0; i++) {
  alpha4.setBrightness(i);
  delay(100);
  }
}

void fadein() {
  for (int i = 0; i < 8; i++) {
  alpha4.setBrightness(i);
  delay(125);
  }
}

// Writes the first four characters at the pointer to the display
void writeArr(Adafruit_AlphaNum4 a, char* buf) {
  a.writeDigitAscii(0, buf[0]);
  a.writeDigitAscii(1, buf[1]);
  a.writeDigitAscii(2, buf[2]);
  a.writeDigitAscii(3, buf[3]);
  a.writeDisplay();
}

void setup() {
  //Serial.begin(9600);
  
  alpha4.begin(0x70);  // pass in the address

  // Clear display to begin
  alpha4.clear();
  alpha4.writeDisplay();
  alpha4.setBrightness(0);

  // Load our display buffer with some initial characters
  char displaybuffer[20] = {'X','X','X','X',' ',' ',' ',' ',' ',' ','H','E','L','L','O',' ','R','I','C','K'};

  // Write first four chars to display and fade in.
  writeArr(alpha4, displaybuffer);
  fadein();

  delay(2000);

  // Scroll through buffer for startup message
  for (int i = 4; i < 17; i++){
    writeArr(alpha4, displaybuffer + i);
    delay(250);
  }

  // Generate scrolling LEDs
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 32; j++) {
      analogWrite( 9,  logSineTable[  j       ]);
      analogWrite(10,  logSineTable[ (j + 6) % 32]);
      analogWrite(11,  logSineTable[ (j + 11) % 32]);
      delay(25);
    }
  }

  // Test code for using sprintf to generate display string from a number
  for (int i = 0; i < 256; i++) {

    // Print out dimension 
    sprintf(displaybuffer, "C%03u", i);
    
    writeArr(alpha4, displaybuffer);
    
    delay(100);
  
  }
  
}

void loop() {
  // Nothing to see here
}