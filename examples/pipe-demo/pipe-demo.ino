#include <FastLED.h>

#include <NixiePipe.h>

#define LED_PIN       6
#define NUM_PIPES     4
#define BRIGHTNESS    255
#define FADE_DEC	    20           // Fade effect delay
#define INC_DELAY     200          // Delay between number increment in cycles
#define MAIN_COLOUR   CRGB::OrangeRed

#define FRAMES_PER_SECOND  128
#define SEQUENCE_TIME      15      // Time between sequences

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0;                  // Rainbow hue inc.
uint8_t i = 0;                     // Master number inc.
uint32_t pipesMax = 0;             // Max number pipes can show set by pipes.getMax()
volatile bool incFlag = false;

void cycleUp();
void cycleDown();
void counter();
void cycleBlock();
void shifter();
void rainbow();
void rainbowWithGlitter();
void confetti();
void sinelon();
void juggle();

NixiePipe pipes = NixiePipe(NUM_PIPES,LED_PIN);

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
#if (NUM_PIPES > 3)
  SimplePatternList gPatterns = { cycleUp, cycleDown, cycleBlock, shifter, rainbow, rainbowWithGlitter, confetti, sinelon, juggle };
#else
  SimplePatternList gPatterns = { shifter, cycleBlock, rainbow, rainbowWithGlitter, confetti, sinelon, juggle };
#endif

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void setup() {
  Serial.begin(9600);
  pipes.passSerial(Serial);
  pipes.clear();
  pipes.setBrightness(BRIGHTNESS);
  pipes.setPipeColour(MAIN_COLOUR);
  pipes.writeNumber(1);
  pipes.show();
  pipesMax = pipes.getMax();
}

void loop() {
    // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();
  incFlag = false;

	// do some periodic updates
	EVERY_N_MILLISECONDS(INC_DELAY) { (i < pipesMax) ? ++i : i=0; incFlag = true; }
	EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
	EVERY_N_SECONDS( SEQUENCE_TIME ) { nextPattern(); } // change patterns periodically

	pipes.show();

  FastLED.delay(1000/FRAMES_PER_SECOND);
}

void nextPattern()
{
	// add one to the current pattern number, and wrap around at the end
	gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
	if (gCurrentPatternNumber == 0) {
    pipes.writeNumber(i,MAIN_COLOUR);
  }
}

void cycleUp() {
  if (incFlag)
    ++pipes;
	pipes.write();
}

void cycleDown() {
	if (incFlag)
    --pipes;
	pipes.write();
}

void counter() {
  pipes.writeNumber(i);
}

void cycleBlock() {
  for (int p = 0; p < NUM_PIPES; p++)
    pipes.setPipeNumber(p, i % 10);
    pipes.writeFade(FADE_DEC);
}

void shifter() {
	if (incFlag)
    pipes.shift(1);
	pipes.setPipeNumber(0,i % 10);
	pipes.write();
}

void rainbow()
{
	cycleBlock();
	pipes.writeRainbow(gHue);
}

void rainbowWithGlitter()
{
	addGlitter(8);
	rainbow();
}

void addGlitter( fract8 chanceOfGlitter)
{
	CRGB* leds = pipes.getPixels();
	if( random8() < chanceOfGlitter) {
		leds[ random16(NUM_PIPES*9) ] += CRGB::White;
	}
}

void confetti()
{
	CRGB rgb;

	pipes.setNumber(i);
	hsv2rgb_rainbow(CHSV( gHue + random8(64), 200, 255),rgb);
  pipes.setPipeColour(rgb);
	pipes.writeFade(FADE_DEC);
}

void sinelon()
{
	CRGB rgb;

	if (incFlag)
    pipes.setNumber(beatsin16(13,0,pipesMax));

	hsv2rgb_rainbow(CHSV( gHue, 255, 192),rgb);
  pipes.setPipeColour(rgb);
	pipes.writeFade(FADE_DEC);
}

void juggle() {
	byte dothue = 0;
	CRGB rgb;
	for( int i = 0; i < NUM_PIPES; i++) {
		pipes.setPipeNumber( i,beatsin16(i+7,0,10) );
		hsv2rgb_rainbow(CHSV(dothue, 200, 255),rgb);
    pipes.setPipeColour(i,rgb);
		dothue += 32;
	}
  pipes.writeFade(128);
}

