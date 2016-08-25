#include <FastLED.h>

#include <NixiePipe.h>

#define LED_PIN       6
#define NUM_PIPES     1
#define BRIGHTNESS    255
#define FADE_DEC	    20

#define FRAMES_PER_SECOND  128

NixiePipe pipes = NixiePipe(NUM_PIPES,LED_PIN);
uint8_t upFlag = false;
uint8_t downFlag = false;
uint8_t gHue = 0;

static void setISR(void) {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  // disable interrupts until acted on
  noInterrupts();

  // debouce
  if (interrupt_time - last_interrupt_time > 200) {
    if (digitalRead(2) == LOW) {
      upFlag = true;
    } else if (digitalRead(3) == LOW) {
      downFlag = true;
    }
  } else {
    interrupts();
  }

  last_interrupt_time = interrupt_time;
}

void setup() {
  attachInterrupt(0, setISR, FALLING);
  attachInterrupt(1, setISR, FALLING);
  pinMode(2,INPUT);
  pinMode(3,INPUT);

  pipes.clear();
  pipes.setBrightness(BRIGHTNESS); 
  pipes.show();
}

void loop() {

  if (upFlag) {
    ++pipes;
    upFlag = false;
  }

  if (downFlag) {
    --pipes;
    downFlag = false;
  }

  pipes.writeFade(FADE_DEC);
  pipes.writeRainbow(gHue);
	pipes.show();
	FastLED.delay(1000/FRAMES_PER_SECOND);

	// do some periodic updates
	EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
}
