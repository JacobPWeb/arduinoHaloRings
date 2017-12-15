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
Adafruit_NeoPixel leftRing(16, 2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel rightRing(16, 3, NEO_GRB + NEO_KHZ800);
Animations strip1(leftRing, NUMPIXELS);
Animations strip2(rightRing, NUMPIXELS);
int green = 255;
int red = 255;
int blue = 255;
char *colours[10];

void setup() {
  bluetooth.begin(9600);
  strip2.lightSetup();
  strip2.setActivePattern(DRL);
  strip1.lightSetup();
  strip1.setActivePattern(DRL);
  pinMode(rightIndicatorInput, INPUT);
  pinMode(leftIndicatorInput, INPUT);
}

void readColorValues() {
  /*static byte ndx = 0;
  char endMarker = ';';
  char colorSeparator = ',';
  char rc;
  char *token;
  while (bluetooth.available() > 0 && newData == false) {
    rc = bluetooth.read();
    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    } else {
      receivedChars[ndx] = '\0';
      ndx = 0;
      green = atoi(strtok(receivedChars, colorSeparator));
      blue = atoi(strtok(receivedChars, colorSeparator));
      red = atoi(strtok(receivedChars, colorSeparator));
      
    }
  }*/
  green = bluetooth.readStringUntil(',').toInt();
  blue = bluetooth.readStringUntil(',').toInt();
  red = bluetooth.readStringUntil(',').toInt();
}

void updateColorValues() {
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



void parseCommand() {
  if (newData == true) {
    if (strcmp("color", receivedChars) == 0) {
      // readColorValues();
      updateColorValues();
      strip1.setActivePattern(SOLID);
      strip2.setActivePattern(SOLID);
      bluetooth.println(receivedChars);
      bluetooth.println(green);
      bluetooth.println(red);
      bluetooth.println(blue);
      return;
    }
    else if (strcmp("solid", receivedChars) == 0) {
      strip1.setActivePattern(SOLID);
      strip2.setActivePattern(SOLID);
      updateColorValues();
    }
    else if (strcmp("chase", receivedChars) == 0) {
      strip1.setActivePattern(CHASE);
      strip2.setActivePattern(CHASE);
      updateColorValues();
    } 
    else if (strcmp("rainbow", receivedChars) == 0) {
      strip1.setActivePattern(RAINBOW);
      strip2.setActivePattern(RAINBOW);
    } 
    else if (strcmp("drl", receivedChars) == 0) {
      strip1.setActivePattern(DRL);
      strip2.setActivePattern(DRL);
    }
    else if (strcmp("stars", receivedChars) == 0) {
      strip1.setActivePattern(STARS);
      strip2.setActivePattern(STARS);
      updateColorValues();
    }
    else if (strcmp("off", receivedChars) == 0) {
      strip1.setActivePattern(OFF);
      strip2.setActivePattern(OFF);
      bluetoothOverride = false;
    } 
    else if (strcmp("randomfill", receivedChars) == 0) {
      strip1.setActivePattern(RANDOMFILL);
      strip2.setActivePattern(RANDOMFILL);
    } else {
      return; 
    }
    strip1.turnOffRing();
    strip2.turnOffRing();
    newData = false;
    strip1.resetBaseOperations();
    strip1.update();
    strip2.resetBaseOperations();
    strip2.update();
  }
}

void loop() {
   if(bluetooth.available() == 0) {
     strip1.update();
     strip2.update();
   } else {
     recieveWithEndMarker();
     parseCommand();
   }

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
       newData = true;
       green = bluetooth.readStringUntil(',').toInt();
       blue = bluetooth.readStringUntil(',').toInt();
       red = bluetooth.readStringUntil(',').toInt();
    } 
    else {
      receivedChars[ndx] = '\0';
      ndx = 0;
      newData = true;
    }
  }
}
