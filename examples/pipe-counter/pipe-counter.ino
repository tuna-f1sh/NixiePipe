#include <FastLED.h>

#include <NixiePipe.h>

#define LED_PIN       6
#define NUM_PIPES     8
#define BRIGHTNESS    255
#define FADE_DEC	    64

#define INC_RATE      500
#define DIR_UP        true

#define RAINBOW       true

NixiePipe pipes = NixiePipe(NUM_PIPES);
uint8_t gHue = 0;

void setup() {
  pipes.begin<LED_PIN>();
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
