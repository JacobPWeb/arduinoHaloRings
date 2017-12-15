#include <Arduino.h>
#include <SoftwareSerial.h>
#include <FastLED.h>

enum pattern { RAINBOW, INDICATE, DRL, DEMO, SOLID, NONE, EMPTY, CHASE, OFF, RANDOMFILL, STARS };

class Animations {
  public:
    int NUMPIXELS;
    boolean colorChanged = false;
    int interval = 100;
    int green = 0;
    int red = 0;
    int blue = 0;
    unsigned long lastUpdate;
    pattern activePattern;
    int index = 0;
    int subIndex = 0;
    CRGB* leds;
    uint8_t gHue = 0;
    
    Animations (int pixelStripSize, CRGB* ledsArray) {
      NUMPIXELS = pixelStripSize;
      leds = ledsArray;      
    }
    
    void setActivePattern (pattern patternValue) {
      activePattern = patternValue;
      switch (activePattern) {
        case DRL:
          interval = 50;
          break;
        case SOLID:
          interval = 50;
          break;
        case INDICATE:
          interval = 50;
          break;
        case DEMO:
          break;
        case EMPTY:
          interval = 100;
          break;
        case STARS:
          interval = 40;
          break;
        case RAINBOW:
          interval = 10;
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
    
    int getInterval() {
       return interval; 
    }
    
    void update() {   
      if (!colorChanged) {
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
          case STARS:
            stars();
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
    
    void stars() {
      
    }

    void fillUpRing() {
      if (index == 0) {
          leds[index].setRGB(green, red, blue);
          FastLED.show();
          index++;
        } 
        else if (index < ( ( NUMPIXELS / 2 ) + 1 )) {
          leds[index].setRGB(green, red, blue);
          leds[NUMPIXELS - index].setRGB(green, red, blue);
          FastLED.show();
          index++;
        } 
        else if ( index == ( ( NUMPIXELS / 2 ) + 1 )) {
          index = 0;
          colorChanged = true;
        }
    }
    
    void resetBaseOperations () {
      colorChanged = false;
      lastUpdate = 0;
      index = 0;
      subIndex = 0;
      turnOffRing();
    }

    void emptyRing() {
      if (index == 0) {
        leds[NUMPIXELS / 2] = CRGB::Black;
        FastLED.show();
        index++;
      } 
      else if (index != 0 && index < ( ( NUMPIXELS / 2 ) + 1 )) {
        leds[(NUMPIXELS / 2) - index] = CRGB::Black;
        leds[(NUMPIXELS / 2) + index] = CRGB::Black;
        FastLED.show();
        index++;
      } 
      else if ( index == ( ( NUMPIXELS / 2 ) + 1 )) {
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
      for(int i = 0; i < NUMPIXELS; i++) {
        leds[i] = CRGB::Black; 
      }
      FastLED.show();
      colorChanged = true;
    }
    
    void randomColorFill() {
     if (subIndex == 0 && index == 0) {
      changeColorsRandom(); 
     }
     if (subIndex < NUMPIXELS - index) {
       leds[subIndex-1] = CRGB::Black;
       leds[subIndex].setRGB(green, red, blue);
       FastLED.show();
       subIndex++;
     } 
     else if ( index < NUMPIXELS ) {
       index++;
       subIndex = 0;
     } 
     else {
       FastLED.clear();
       FastLED.show();
       index = 0;
       subIndex = 0; 
     }
   }
    
    void rainbowCycle() {
      if(gHue == 255) gHue = 0;
      fill_rainbow( leds, NUMPIXELS, gHue, 17 );
      gHue++;
      FastLED.show();
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
        FastLED.clear();
        for (int i = 0; i < 6; i++) {
          int newGreen = green/i;
          int newRed = red/i;
          int newBlue = blue/i;
          int newIndex = index - i;
          if (newIndex < 0) {
            newIndex = NUMPIXELS - abs(newIndex);
          }
          CHSV conversion = rgb2hsv_approximate(CRGB(newRed, newBlue,newGreen));
          conversion.value = 255/i;
          leds[newIndex] = conversion;
        }
        FastLED.show();
        index++;
      } 
      else {
        index = 0; 
        chasingAnimation();
      }
    }

};
