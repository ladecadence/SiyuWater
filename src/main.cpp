#include <Arduino.h>
#include "FastLED.h"

#define NUMLEDS 60
#define THRESHOLD 45
#define LEDPIN  15

#define COOLING  90
#define SPARKING 100

#define NUM_PALETTE 16

#define BRIGHT_LOW 3
#define BRIGHT_HIGH 30

enum state {
  STATE_DIM,
  STATE_BRIGHT
};

byte palette[] {
  3,26,34,
  4,32,40,
  5,37,47,
  5,42,54,
  6,47,60,
  7,53,67,
  7,58,74,
  8,63,80,
  8,68,87,
  9,74,94,
  10,79,101,
  12,100,127,
  15,118,151,
  21,170,217,
  40,188,234,
  73,198,237,
};

CRGB leds[NUMLEDS];
int last_value = 0;
byte brightness = BRIGHT_LOW;
state st = STATE_DIM;

CRGB get_palette_color(int index) {
  CRGB col;
  if (index>=0 && index < NUM_PALETTE) {
    col.r = palette[index*3];
    col.g = palette[index*3 + 1];
    col.b = palette[index*3 + 2]; 
  } else {
    col.r = col.b = col.g = 0;
  }
  return col;
}

void showStrip() {
   FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUMLEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}

void wave_init() {
  for (int i = 0; i<NUMLEDS; i++) {
    CRGB col = get_palette_color(rand() % 16);
    setPixel(i, col.r, col.g, col.b);
  }
  showStrip();
}

void wave_init_soft() {
   for (int i = 1; i<NUMLEDS; i+=3) {
    CRGB col = get_palette_color(rand() % 16);
    setPixel(i, col.r, col.g, col.b);
    setPixel(i+1, col.r, col.g, col.b);
    setPixel(i-1, col.r, col.g, col.b);
    leds[i+1].fadeLightBy(128);
    leds[i-1].fadeLightBy(128); 
  }
}

void wave_walk(int speedDelay) {
  for (int i=0; i<NUMLEDS; i++) {
    // move down
    if (i < NUMLEDS-1) {
      setPixel(i, leds[i+1].r, leds[i+1].g, leds[i+1].b);
    } else {
      CRGB col = get_palette_color(rand() % 16);
      setPixel(i, col.r, col.g, col.b);
    }
  }
  showStrip();
  delay(speedDelay);
}

void setup() {
  Serial.begin(115200);
  delay(1000); // give me time to bring up serial monitor
  srand(millis());
  Serial.println("Starting...");
  FastLED.addLeds<NEOPIXEL, LEDPIN>(leds, NUMLEDS);
  FastLED.setBrightness(brightness);
  wave_init_soft();
}

void loop() {
  wave_walk(50);

  last_value = touchRead(4);
  if (last_value  < THRESHOLD) {
    st = STATE_BRIGHT;
  } else {
    st = STATE_DIM;
  }

  // change brightness?
  if (st == STATE_BRIGHT && FastLED.getBrightness() == BRIGHT_LOW) {
    FastLED.setBrightness(BRIGHT_HIGH);
  } else if (st == STATE_DIM && FastLED.getBrightness() == BRIGHT_HIGH) {
    FastLED.setBrightness(BRIGHT_LOW);
  }

}
