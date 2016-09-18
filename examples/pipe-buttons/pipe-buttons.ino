#include <FastLED.h>

#include <NixiePipe.h>

#define LED_PIN       6
#define NUM_PIPES     2
#define BRIGHTNESS    255
#define FADE_DEC	    20

#define DEBOUNCE       200
#define FRAMES_PER_SECOND  128

NixiePipe pipes = NixiePipe(NUM_PIPES,0,LED_PIN);
uint8_t gHue = 0;

void setup() {
  pinMode(PIPE_TB0,INPUT); // TB0
  pinMode(PIPE_TB1,INPUT); // TB1

  pipes.clear();
  pipes.setBrightness(BRIGHTNESS); 
  pipes.show();
}

void loop() {
  static unsigned long tb0db = 0;
  unsigned long tb0int = millis();
  static unsigned long tb1db = 0;
  unsigned long tb1int = millis();

  if (~digitalRead(PIPE_TB0) & (tb0int - tb0db > DEBOUNCE)) {
    ++pipes;
    tb0db = tb0int;
  }

  if (~digitalRead(PIPE_TB1) & (tb1int - tb1db > DEBOUNCE)) {
    --pipes;
    tb1db = tb1int;
  }

  pipes.writeFade(FADE_DEC);
  pipes.writeRainbow(gHue);
	pipes.show();

	// do some periodic updates
	EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow

	/* FastLED.delay(1000/FRAMES_PER_SECOND);*/
	delay(1000/FRAMES_PER_SECOND);

}
