// Code for some Rick and Morty portal gun fun
// Dan Kouba, 2016

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#include <Encoder.h>
#define ENCODER_OPTIMIZE_INTERRUPTS

#define DIM_MAX 199
#define DIM_MIN 0

// Display
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

// Encoder
Encoder knob(2, 3);

// Values adjusted by the encoder
int  dimensionNum    = 132;
char dimensionChar   = 'C';

// Process knob movement
// Dimension # is bounded between 0 and 200
// ASCII ID char is dimension number % 200, bounded A to Z
void processKnob() {
  int pos = knob.read();
  
  if (pos < DIM_MIN) {
    //Modulo 200 negative transition
    if (dimensionChar > 'A') {
      dimensionChar--;
      pos += DIM_MAX;
    } 
    // Lower bound
    else {
      pos = DIM_MIN;
    }
    knob.write(pos);
  }
  else if (pos >= DIM_MAX) {
    // Modulo 200 positive transition
    if (dimensionChar < 'Z') {
      dimensionChar++;
      pos -= DIM_MAX;
    }
    // Upper bound
    else {
      pos = DIM_MAX;
    }
    knob.write(pos);
  }
  
  if (pos != dimensionNum) {
    writeDim(alpha4, dimensionChar, pos);
    dimensionNum = pos;
  }
}

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
  for (int i = 7; i >=0; i--) {
  alpha4.setBrightness(i);
  delay(125);
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

// Print out dimension 
void writeDim(Adafruit_AlphaNum4 a, char chr, int dim){
  char buf[4];
  sprintf(buf, "%c%03u", chr, dim);
  writeArr(alpha4, buf);
}

// Generate scrolling LEDs
void portalLED(uint8_t cycles) {
  for (int i = 0; i < cycles; i++) {
    for (int j = 0; j < 32; j++) {
      analogWrite( 9,  logSineTable[  j       ]);
      analogWrite(10,  logSineTable[ (j + 6) % 32]);
      analogWrite(11,  logSineTable[ (j + 11) % 32]);
      delay(25);
    }
    analogWrite( 9, 0);
    analogWrite(10, 0);
    analogWrite(11, 0);
  }
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


  // Test code for using sprintf to generate display string from a number
  for (int i = 0; i < 256; i++) {

    // Print out dimension 
    sprintf(displaybuffer, "C%03u", i);
    
    writeArr(alpha4, displaybuffer);
    
    delay(100);
  
  }
  
  // Clear any rotations up to this point
  knob.write(dimensionNum);
}

void loop() {
  processKnob();
}
