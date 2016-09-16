#include <FastLED.h>

#include <NixiePipe.h>

#define LED_PIN       6
#define NUM_PIPES     4
#define BRIGHTNESS    255
#define FADE_DEC	    32

#define INC_RATE      100
#define DIR_UP        true

#define RAINBOW       false

NixiePipe pipes = NixiePipe(NUM_PIPES,LED_PIN);
uint8_t gHue = 0;

void setup() {
  pipes.clear();
  pipes.setBrightness(BRIGHTNESS); 
  pipes.show();
}

void loop() {
  pipes.writeFade(FADE_DEC);
  if (RAINBOW)
    pipes.writeRainbow(gHue);
	pipes.show();

	// do some periodic updates
	EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
	EVERY_N_MILLISECONDS( INC_RATE ) { DIR_UP ? ++pipes : --pipes; } // slowly cycle the "base color" through the rainbow
}