#include <Wire.h>
#include <TimeLib.h>
#include <FastLED.h>
#include <DS3232RTC.h>

#include <NixiePipe.h>

#define LED_PIN       6
#define NUM_PIPES     4
#define NUM_UNITS     0
#define BRIGHTNESS    255

#define COLOUR        CRGB::White
#define RAINBOW       true // display rainbow digits

uint8_t gHue = 0;

NixiePipe pipes = NixiePipe(NUM_PIPES,NUM_UNITS,LED_PIN);

static inline void setTime(tmElements_t tm) {
  if (NUM_PIPES <= 5)
    pipes.setNumber((tm.Hour * 100) + tm.Minute);
  else
    pipes.setNumber((tm.Hour * 10000) + (tm.Minute * 100) + tm.Second);
}

void setup() {
  pipes.clear();
  pipes.setBrightness(BRIGHTNESS);
  pipes.setPipeColour(COLOUR);
  pipes.writeNumber(0);
  pipes.show();
}

void loop() {
  tmElements_t tm; // time struct holder

  EVERY_N_SECONDS(1) {
    if (RTC.read(tm) == 0) {
      setTime(tm);
    } else {
      // rtc isn't reading
      pipes.setNumber(0);
      pipes.writeSolid(CRGB::Red); // all red
      pipes.show();
      return;
    }
  }
  
  EVERY_N_MILLISECONDS( 50 ) { gHue++; } // slowly cycle the "base color" through the rainbow

  pipes.writeFade(4);
  if (RAINBOW) 
    pipes.writeRainbow(gHue);

  pipes.show();
}
