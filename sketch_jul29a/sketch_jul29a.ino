/* Includes */
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include "animations.cpp";

/* Setup */
#define NUMPIXELS      16
#define NUMRINGS       2

/* Bluetooth comms */
int bluetoothTx = 10;  // TX-O pin of bluetooth
int bluetoothRx = 9;  // RX-I pin of bluetooth
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);
boolean rainbow = false;

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
Adafruit_NeoPixel leftRing(16, 2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRing(16, 3, NEO_GRB + NEO_KHZ800);
Animations strip1(leftRing, NUMPIXELS);
Animations strip2(rightRing, NUMPIXELS);

void setup() {
  bluetooth.begin(9600);
  strip2.lightSetup();
  strip2.setActivePattern(RANDOMFILL);
  strip1.lightSetup();
  strip1.setActivePattern(RANDOMFILL);
  pinMode(rightIndicatorInput, INPUT);
  pinMode(leftIndicatorInput, INPUT);
}

void readColorValues() {
  String bluetoothVal = bluetooth.readString();
  int green = getValue(bluetoothVal, ',', 0).toInt();
  int blue = getValue(bluetoothVal, ',', 1).toInt();
  int red = getValue(bluetoothVal, ',', 2).toInt();
  strip1.changeColors(green, red, blue);
  strip2.changeColors(green, red, blue);
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;

void parseCommand() {
  if (newData == true) {
    if (strcmp("color", receivedChars) == 0) {
      //readColorValues();
      //strip1.setActivePattern(SOLID);
      //strip2.setActivePattern(SOLID);
    } else if (strcmp("chase", receivedChars) == 0) {
      strip1.setActivePattern(CHASE);
      strip2.setActivePattern(CHASE);
    } else if (strcmp("rainbow", receivedChars) == 0) {
      strip1.setActivePattern(RAINBOW);
      strip2.setActivePattern(RAINBOW);
    } else if (strcmp("drl", receivedChars) == 0) {
      strip1.setActivePattern(DRL);
      strip2.setActivePattern(DRL);
    } else if (strcmp("off", receivedChars) == 0) {
      strip1.setActivePattern(OFF);
      strip2.setActivePattern(OFF);
      bluetoothOverride = false;
    } else if (strcmp("randomfill", receivedChars) == 0) {
      strip1.setActivePattern(RANDOMFILL);
      strip2.setActivePattern(RANDOMFILL);
    }
    newData = false;
    strip1.resetBaseOperations();
    strip1.update();
    strip2.resetBaseOperations();
    strip2.update();
  }
}

void loop() {
   strip1.update();
   strip2.update();
   recieveWithEndMarker();
   parseCommand();

    if ( digitalRead(rightIndicatorInput) == 1 && rightChanged != true) {
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
    }
}

void recieveWithEndMarker() {
  static byte ndx = 0;
  char endMarker = ';';
  char rc;
  while (bluetooth.available() > 0 && newData == false) {
    rc = bluetooth.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      strip1.turnOffRing();
      strip2.turnOffRing();
      receivedChars[ndx] = '\0';
      ndx = 0;
      newData = true;
    }
  }
}
