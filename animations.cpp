#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

enum pattern { RAINBOW, INDICATE, DRL, DEMO, SOLID, NONE, EMPTY, CHASE, OFF, RANDOMFILL };

class Animations {
  public:
    int NUMPIXELS;
    boolean colorChanged = false;
    Adafruit_NeoPixel ledStrip;
    int interval = 100;
    int green = 0;
    int red = 0;
    int blue = 0;
    unsigned long lastUpdate;
    unsigned long lastRainbowUpdate;
    pattern activePattern;
    int index = 0;
    int subIndex = 0;
    
    Animations (Adafruit_NeoPixel strip, int pixelStripSize) {
      NUMPIXELS = pixelStripSize;
      ledStrip = strip;
    }

    void update() {   
      if (!colorChanged) {
        if ( (millis() - lastUpdate) > interval) {
          lastUpdate = millis();
          switch (activePattern) {
            case DRL:
              changeColors(255, 255, 255);
              fillUpRing();
              break;
            case SOLID:
              fillUpRing();
              break;
            case INDICATE:
              changeColors(255, 100, 0);
              fillUpRing();
              break;
            case DEMO:
              break;
            case EMPTY:
              emptyRing();
              break;
            case RANDOMFILL:
              randomColorFill();
              break;
            case CHASE:
              chasingAnimation();
              break;
            case OFF:
              turnOffRing();
              break;
            case RAINBOW:
              rainbowCycle();
              break;
            default:
              break;
          }
        }
      }
    }

    void fillUpRing() {
      if (index == 0) {
        turnOffRing();
        colorChanged = false;
        ledStrip.setPixelColor(0, ledStrip.Color(green, red, blue));
        ledStrip.show();
        index++;
      } else if (index != 0 && index < ( ( NUMPIXELS / 2 ) + 1 )) {
        ledStrip.setPixelColor(index, ledStrip.Color(green, red, blue));
        ledStrip.setPixelColor(NUMPIXELS - index, ledStrip.Color(green, red, blue));
        ledStrip.show();
        index++;
      } else if ( index == ( ( NUMPIXELS / 2 ) + 1 )) {
        index = 0;
        colorChanged = true;
      }
    }

    void setActivePattern (pattern patternValue) {
      activePattern = patternValue;
      switch (activePattern) {
        case DRL:
          interval = 50;
          break;
        case SOLID:
          interval = 75;
          break;
        case INDICATE:
          interval = 50;
          break;
        case DEMO:
          break;
        case EMPTY:
          interval = 100;
          break;
        case RAINBOW:
          interval = 0;
          break;
        case RANDOMFILL:
          interval = 30;
          break;
        case CHASE:
          interval = 30;
          break;
        case OFF:
          break;
        default:
          return;
          break;
      }     
    }
    
    void resetBaseOperations () {
      colorChanged = false;
      lastUpdate = 0;
      index = 0;
      subIndex = 0;
    }
    
    
  
    void lightSetup() {
      ledStrip.begin();
      ledStrip.setBrightness(100);
      ledStrip.show();
    }

    void emptyRing() {
      if (index == 0) {
        ledStrip.setPixelColor((NUMPIXELS / 2), ledStrip.Color(0, 0, 0));
        ledStrip.show();
        index++;
      } else if (index != 0 && index < ( ( NUMPIXELS / 2 ) + 1 )) {
        ledStrip.setPixelColor((NUMPIXELS / 2) - index, ledStrip.Color(0, 0, 0));
        ledStrip.setPixelColor((NUMPIXELS / 2) + index, ledStrip.Color(0, 0, 0));
        ledStrip.show();
        index++;
      } else if ( index == ( ( NUMPIXELS / 2 ) + 1 )) {
        index = 0;
        colorChanged = true;
      }
    }
    
    void changeColors (int g, int r, int b) {
      green = g;
      red = r;
      blue = b;
    }
    
    void changeColorsRandom () {
      red = random(0,255); 
      green = random(0,255);
      blue = random(0,255);
    }
    
    void turnOffRing () {
      ledStrip.clear();
      ledStrip.show();
      colorChanged = true;
    }
    
    void randomColorFill() {
     if (index == 0) {
      changeColorsRandom(); 
     }
     if (subIndex < NUMPIXELS - index) {
       ledStrip.setPixelColor(subIndex-1, ledStrip.Color(0, 0, 0));
       ledStrip.setPixelColor(subIndex, ledStrip.Color(green, red, blue));
       ledStrip.show();
       subIndex++;
     } else if ( index < NUMPIXELS ) {
       index++;
       subIndex = 0;
       changeColorsRandom(); 
     } else {
       ledStrip.clear();
       ledStrip.show();
       index = 0;
       subIndex = 0; 
     }
   }
    
    void rainbowCycle() {
      if (subIndex < 256) {  
        if (index < NUMPIXELS) {
          int wheelPos = ( (index * 256 / NUMPIXELS) + subIndex) & 255;
          ledStrip.setPixelColor(index, Wheel(wheelPos) );
          index++;
          ledStrip.show();
          if ( (millis() - lastRainbowUpdate) > 2) {
            subIndex++;
            lastRainbowUpdate = millis();
          }
        } else {
         index = 0; 
        }
      } else {
        subIndex = 0;
      }
    }
        
    int getChasingIndex (int index, int increment) {
      int newIndex = index - increment;
      if (newIndex < 0) {
       return (NUMPIXELS - increment);
      }
      return newIndex;
    }
    
    void chasingAnimation () {
      if (index < NUMPIXELS) {
        ledStrip.clear();
        ledStrip.show();
        for (int i = 0; i < 6; i++) {
          int newColor = 250/i;
          int newIndex = index - i;
          if (newIndex < 0) {
            newIndex = NUMPIXELS - abs(newIndex);
          }
          ledStrip.setPixelColor( newIndex, ledStrip.Color(0, newColor, 0) );
        }
        ledStrip.show();
        index++;
      } else {
        index = 0; 
        chasingAnimation();
      }
    }


  private:
    uint32_t Wheel(int WheelPos) {
      int wheelPos3 = WheelPos * 3;
      int red;
      int blue;
      int green;
      if (WheelPos < 85) {
        green = wheelPos3;
        red = 255 - wheelPos3;
        blue = 0;
      } 
      else if (WheelPos < 170) {
        int newWheelPos = WheelPos - 85;
        wheelPos3 = newWheelPos * 3;
        green = 255 - wheelPos3;
        red = 0;
        blue = wheelPos3;
      } 
      else {
        int newWheelPos = WheelPos - 170;
        wheelPos3 = newWheelPos * 3;
        green = 0;
        red = wheelPos3;
        blue = 255 - wheelPos3;
      }
      return ledStrip.Color(green, red, blue);
    }

};
