/* Includes */
#include <FastLED.h>
#include <SoftwareSerial.h>
#include "animations.cpp";
/* Setup */
#define NUMPIXELS 16
#define COLOR_ORDER GRB
#define LED_TYPE WS2812B

/* Bluetooth comms */
int bluetoothTx = 10;  // TX-O pin of bluetooth
int bluetoothRx = 9;  // RX-I pin of bluetooth
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;


/* DRL and Indicator */
boolean leftIsIndicating = false;
boolean rightIsIndicating = false;
boolean rightChanged = false;
boolean leftChanged = false;
unsigned long leftLastIndicated = 0;
unsigned long rightLastIndicated = 0;
int rightIndicatorInput = 4;
int leftIndicatorInput = 5;
boolean bluetoothOverride = false;

/* Fog light rings */
int green = 255;
int red = 255;
int blue = 255;

CRGB leds[NUMPIXELS];

Animations animation(16, leds);

void setup() {
  bluetooth.begin(9600);
  delay(1000);
  animation.setActivePattern(CHASE);
  pinMode(rightIndicatorInput, INPUT);
  pinMode(leftIndicatorInput, INPUT);
  FastLED.addLeds<LED_TYPE, 3, COLOR_ORDER>(leds, NUMPIXELS);
  FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(leds, NUMPIXELS);
}

boolean colorChanged = false;
int index = 0;
int interval = 10;


void loop() {
  recieveWithEndMarker();
  EVERY_N_MILLISECONDS(animation.getInterval()) {
    animation.update();
  }
  //if (colorChanged) {
  //  EVERY_N_MILLISECONDS(interval) {
  //    fillUpRing();
  //  }
  //}

    /*if ( digitalRead(rightIndicatorInput) == 1 && rightChanged != true) {
      strip2.setActivePattern(INDICATE);
      strip2.resetBaseOperations();
      strip2.update();
      rightChanged = true;
    } else if (rightChanged == true) {
      if ( digitalRead(rightIndicatorInput) == 0) {
          rightLastIndicated = millis();
          strip2.setActivePattern(OFF);
          strip2.resetBaseOperations();
          strip2.update();
          rightChanged = false;
      }
    }
    if (rightLastIndicated != 0) {
      if ( digitalRead(rightIndicatorInput) == 0 && (millis() - rightLastIndicated) > 1500) {
        rightLastIndicated = 0;
        strip2.setActivePattern(DRL);
        strip2.resetBaseOperations();
        strip2.update();
      }
    }
    
    if ( digitalRead(leftIndicatorInput) == 1 && leftChanged != true) {
      strip1.setActivePattern(INDICATE);
      strip1.resetBaseOperations();
      strip1.update();
      leftChanged = true;
    } else if (leftChanged == true) {
      if ( digitalRead(leftIndicatorInput) == 0) {
          leftLastIndicated = millis();
          strip1.setActivePattern(OFF);
          strip1.resetBaseOperations();
          strip1.update();
          leftChanged = false;
      }
    }
    if (leftLastIndicated != 0) {
      if ( digitalRead(leftIndicatorInput) == 0 && (millis() - leftLastIndicated) > 1500) {
        leftLastIndicated = 0;
        strip1.setActivePattern(DRL);
        strip1.resetBaseOperations();
        strip1.update();
      }
    }*/
}

void updateColorValues() {
  animation.changeColors(green, red, blue);
}

void recieveWithEndMarker() {
  static byte ndx = 0;
  char endMarker = ';';
  char halfwayMarker = ':';
  char rc;
  while (bluetooth.available() > 0 && newData == false) {
    rc = bluetooth.read();
    if (rc != endMarker && rc != halfwayMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else if (rc == halfwayMarker) {
       receivedChars[ndx] = '\0';
       ndx = 0;
       colorChanged = false;
       green = bluetooth.readStringUntil(',').toInt();
       blue = bluetooth.readStringUntil(',').toInt();
       red = bluetooth.readStringUntil(',').toInt();
    } 
    else {
      green = 255;
      blue = 255;
      red = 255;
      colorChanged = false;
      receivedChars[ndx] = '\0';
      ndx = 0;
    }
  }
}

void parseCommand() {
  if (newData == true) {
    if (strcmp("color", receivedChars) == 0) {
      updateColorValues();
      animation.setActivePattern(SOLID);
      return;
    }
    else if (strcmp("solid", receivedChars) == 0) {
      updateColorValues();
      animation.setActivePattern(SOLID);
    }
    else if (strcmp("chase", receivedChars) == 0) {
      animation.setActivePattern(CHASE);
      updateColorValues();
    } 
    else if (strcmp("rainbow", receivedChars) == 0) {
      animation.setActivePattern(RAINBOW);
    } 
    else if (strcmp("drl", receivedChars) == 0) {
      animation.setActivePattern(DRL);
    }
    else if (strcmp("stars", receivedChars) == 0) {
      animation.setActivePattern(STARS);
      updateColorValues();
    }
    else if (strcmp("off", receivedChars) == 0) {
      animation.setActivePattern(OFF);
      bluetoothOverride = false;
    } 
    else if (strcmp("randomfill", receivedChars) == 0) {
      animation.setActivePattern(RANDOMFILL);
    } else {
      return; 
    }
    newData = false;
    animation.resetBaseOperations();
  }
}
