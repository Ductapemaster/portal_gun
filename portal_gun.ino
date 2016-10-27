// Code for some Rick and Morty portal gun fun
// Dan Kouba, 2016

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <avr/sleep.h>

#include <Encoder.h>
#define ENCODER_OPTIMIZE_INTERRUPTS

// Dimension display bounds
#define DIM_MAX 199
#define DIM_MIN 0

// Pin Definitions
#define P_ENCBTN    2
#define P_ENC1      4
#define P_ENC2      5
#define P_LED_FR    9
#define P_LED_FC    10
#define P_LED_FL    11
#define P_LED_TUBE  13

// Button thresholds
#define BTN_DEBOUNCE_DLY  100
#define BTN_LONG_DLY      100000

// Button States
#define BTN_LONGPRESS     0
#define BTN_SHORTPRESS    1

// Button press counter
long btnDelay = 0;

// Display
Adafruit_AlphaNum4 alpha4 = Adafruit_AlphaNum4();

// Encoder - enables pullup R's on these pins automatically
Encoder knob(P_ENC1, P_ENC2);

// Values adjusted by the encoder
int  dimensionNum;
char dimensionChar;

// 32 sample log sine wave table, since brightness is logarithmic
// Yes I know the top and bottom points repeat
const unsigned char logSineTable[32] = {
  0,  1,  2,  3,  5,  8,   11,  16,
  23,   32,  45, 64, 90, 127, 180, 255,
  255, 180, 127, 90, 64,  45,  32,  23,
  16,   11,   8,  5,  3,  2,  1,  0
};

// Scrolls by at startup
char startupMsg[18] = {
  ' ',' ',' ',' ',
  'H','E','L','L','O',' ','R','I','C','K',
  ' ',' ',' ',' ',};

// Process knob movement
// Dimension # is bounded between DIM_MIN and DIM_MAX defines
// ASCII ID char is dimension number % DIM_MAX, bounded A to Z
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

// Crude way to determine how long the button was pressed for
// Stop incrementing if we've reached our max delay - we drop right into processing if its held down long enough
void processButton() {

  if (digitalRead(P_ENCBTN) == LOW && btnDelay < BTN_LONG_DLY) {
    btnDelay++;
  } else {
    if (btnDelay > BTN_DEBOUNCE_DLY) {
      if (btnDelay < BTN_LONG_DLY) {
        portalLED(6);
      } 
      // Long press, turn off
      else {
        shutdownRoutine();
        goToSleep();
        startupRoutine();
      }
    }
    // We should only reset if digitalRead(5) == HIGH
    btnDelay = 0;
  }
}

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
// Want this to be blocking - record knob position before and restore after
// Blocks other presses from happening by default
void portalLED(uint8_t cycles) {

  // Log current knob position
  long pos = knob.read();

  for (int i = 0; i < cycles; i++) {
    for (int j = 0; j < 32; j++) {
      analogWrite(P_LED_FR, logSineTable[  j       ]);
      analogWrite(P_LED_FC, logSineTable[ (j + 6) % 32]);
      analogWrite(P_LED_FL, logSineTable[ (j + 11) % 32]);
      analogWrite(P_LED_TUBE, logSineTable[random(0, 31)]);   // Random flashing for "lightning" effect
      delay(25);
    }
    
    // TODO: Make fadeout routine
    analogWrite(P_LED_FR, 0);
    analogWrite(P_LED_FC, 0);
    analogWrite(P_LED_FL, 0);
    analogWrite(P_LED_TUBE, 0);
  }

  // Restore knob position from before in case it was rotated during
  knob.write(pos);
}

// Interrupt handler for sleep exit - doesn't need to do anything!
void exitSleepISR(void) {
}

// Sleep code, cribbed from:
// http://www.engblaze.com/hush-little-microprocessor-avr-and-arduino-sleep-mode-basics/
void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  sleep_enable();

  attachInterrupt(0, exitSleepISR, LOW);
  sleep_mode();

  // Sleep break point

  detachInterrupt(0);
  sleep_disable();
}

void startupRoutine() {

  // Initial numbers
  dimensionNum  = 137;
  dimensionChar = 'C';

  // Clear display to begin
  alpha4.clear();
  alpha4.writeDisplay();
  alpha4.setBrightness(7);

  // Scroll through buffer for startup message
  for (int i = 0; i <= 14; i++){
    writeArr(alpha4, startupMsg + i);
    delay(200);
  }

  // Transition to Dimension Display
  delay(500);
  alpha4.setBrightness(0);
  writeDim(alpha4, dimensionChar, dimensionNum);
  fadein();

  // Clear any rotations up to this point
  knob.write(dimensionNum);
}

void shutdownRoutine(){
  char buf[12] = {'B','Y','E',' ','R','I','C','K',' ',' ',' ',' '};
  writeArr(alpha4, buf);
  delay(500);
  writeArr(alpha4, buf + 4);
  fadeout();
  writeArr(alpha4, buf + 8);
  alpha4.clear();
}

void setup() {

  // Test this pullup - is it sufficient?
  pinMode(P_ENCBTN, INPUT_PULLUP);
  
  alpha4.begin(0x70);  // pass in the address

}

void loop() {

  // Powerup in sleep mode
  goToSleep();
  startupRoutine();
  while (1) {
    processButton();
    processKnob();
  }
}
